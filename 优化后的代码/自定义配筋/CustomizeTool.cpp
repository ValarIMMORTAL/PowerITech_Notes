#include "_ustation.h"
#include "CustomizeTool.h"
#include "ExtractFacesTool.h"
#include "SelectRebarTool.h"
#include "PITRebarCurve.h"
#include "ArcRebarTool.h"

bool CustomRebar::CalculateArc(PIT::PITRebarCurve& curve, CPoint3DCR begPt, CPoint3DCR midPt, CPoint3DCR endPt)
{
	bool ret = false;

	BeArcSeg arc(begPt, midPt, endPt);

	CPoint3D cen;
	arc.GetCenter(cen);

	if (arc.GetCenter(cen))
	{
		CPoint3D beg = begPt;
		CPoint3D med = midPt;
		CPoint3D end = endPt;

		CVector3D tan1 = arc.GetTangentVector(beg);
		CVector3D tan2 = arc.GetTangentVector(end);

		CPointVect pv1(beg, tan1);
		CPointVect pv2(end, tan2);

		CPoint3D ip;
		bool isIntersect = pv1.Intersect(ip, pv2);

		double radius = arc.GetRadius();

		RebarVertexP vex;
		vex = &(curve.PopVertices()).NewElement();
		vex->SetIP(beg);

		if (curve.PopVertices().GetSize() == 1)
		{
			vex->SetType(RebarVertex::kStart);      // first IP
		}
		else
		{
			vex->SetType(RebarVertex::kIP); 
		}

		CPoint3D mid = (beg + end) / 2.0;
		CVector3D midVec(cen, mid);
		midVec.Normalize();

		if (isIntersect)
		{
			mid = cen + midVec * radius;

			// it can be on the other size
			CPoint3D mid1 = cen - midVec * radius;

			double d1 = med.Distance(mid1);
			double d2 = med.Distance(mid);

			if (d1 < d2)
			{
				mid = mid1;
				midVec = -midVec;
				// this is big arc we need 4 ips

				CVector3D midTan = midVec.Perpendicular();
				CPointVect pvm(mid, midTan);

				pv1.Intersect(ip, pvm);
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(ip);
				vex->SetType(RebarVertex::kIP);      // 2nd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				mid1 = (beg + mid) / 2.0;
				midVec = mid1 - cen;
				midVec.Normalize();
				mid1 = cen + midVec * radius;

				vex->SetArcPt(0, beg);
				vex->SetArcPt(1, mid1);
				vex->SetArcPt(2, mid);



				pv1.Intersect(ip, pvm);
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(beg);
				vex->SetType(RebarVertex::kIP);      // 3rd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				mid1 = (end + mid) / 2.0;
				midVec = mid1 - cen;
				midVec.Normalize();
				mid1 = cen + midVec * radius;

				vex->SetArcPt(0, mid);
				vex->SetArcPt(1, mid1);
				vex->SetArcPt(2, end);

			}
			else
			{
				// this is less than 90 or equal we need 3 ips
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(ip);
				vex->SetType(RebarVertex::kIP);      // 2nd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				vex->SetArcPt(0, beg);
				vex->SetArcPt(1, mid);
				vex->SetArcPt(2, end);
			}
		}
		else
		{
			// this is half circle - we need 4 ips
			midVec = arc.GetTangentVector(med);
			midVec.Normalize();
			DPoint3d ptMedTan = midVec;
			ptMedTan.Scale(radius);
			ptMedTan.Add(med);
			DPoint3d ptBegTan = tan1;
			ptBegTan.Scale(radius);
			ptBegTan.Add(beg);
			mdlVec_intersect(ip, &DSegment3d::From(beg, ptBegTan), &DSegment3d::From(med, ptMedTan));
			// 			EditElementHandle eeh;
			// 			LineHandler::CreateLineElement(eeh, NULL, DSegment3d::From(beg, ptBegTan), true, *ACTIVEMODEL);
			// 			eeh.AddToModel();
			// 			EditElementHandle eeh1;
			// 			LineHandler::CreateLineElement(eeh1, NULL, DSegment3d::From(med, ptMedTan), true, *ACTIVEMODEL);
			// 			eeh1.AddToModel();
			// 			CPointVect pvm(med, midVec);
			// 			pvm.Intersect(ip, tan1);
			// 			tan1 = ip - beg;
			// 			tan1.Normalize();
			// 
			// 			ip = beg + tan1 * radius;
			// 			midVec = ip - cen;
			// 			midVec.Normalize();
			//			midVec = ip - cen;
			mid = cen + tan1 * radius;
			DEllipse3d circle = DEllipse3d::FromCenterRadiusXY(cen, radius);

			double angle_start = circle.PointToAngle(beg);
			double angle_mid = circle.PointToAngle(mid);

			double angle = (angle_start + angle_mid) / 2;
			// 			if (angle < angle_mid)
			// 				angle += _PI;
			CPoint3D mid1;
			circle.Evaluate(&mid1, 0, angle);

			//			CPoint3D mid1 = cen + midVec;

			vex = &curve.PopVertices().NewElement();
			vex->SetIP(ip);
			vex->SetType(RebarVertex::kIP);      // 2nd IP
			vex->SetRadius(radius);
			vex->SetCenter(cen);

			vex->SetArcPt(0, beg);
			vex->SetArcPt(1, mid1);
			vex->SetArcPt(2, mid);

			// 			ip = end + tan1 * radius; // tan1 and tan2 parallel but tan1 has now the correct direction, do not change to tan2
			// 			midVec = ip - cen;
			// 			midVec.Normalize();
			// 			mid1 = cen + midVec * radius;

			//			midVec.Negate();
			DPoint3d ptEndTan = tan2;
			ptEndTan.Scale(radius);
			ptEndTan.Add(end);
			mdlVec_intersect(ip, &DSegment3d::From(end, ptEndTan), &DSegment3d::From(med, ptMedTan));
			// 			midVec = ip - cen;
			// 			mid1 = cen + midVec;

			// 			EditElementHandle eeh2;
			// 			LineHandler::CreateLineElement(eeh2, NULL, DSegment3d::From(end, ptEndTan), true, *ACTIVEMODEL);
			// 			eeh2.AddToModel();
			double angle_end = circle.PointToAngle(end);

			angle = (angle_end + angle_mid) / 2;
			// 			if (angle < angle_end)
			// 				angle += _PI;

			circle.Evaluate(&mid1, 0, angle);

			vex = &curve.PopVertices().NewElement();
			vex->SetIP(ip);
			vex->SetType(RebarVertex::kIP);      // 3rd IP
			vex->SetRadius(radius);
			vex->SetCenter(cen);

			vex->SetArcPt(0, mid);
			vex->SetArcPt(1, mid1);
			vex->SetArcPt(2, end);
		}

		vex = &curve.PopVertices().NewElement();
		vex->SetIP(end);
		vex->SetType(RebarVertex::kIP);      // last IP

		ret = true;
	}

	return ret;
}

bool CustomRebar::CalculateArc(RebarVertices&  vers, CPoint3DCR begPt, CPoint3DCR midPt, CPoint3DCR endPt)
{
	bool ret = false;
	PIT::PITRebarCurve curve;
	BeArcSeg arc(begPt, midPt, endPt);

	CPoint3D cen;
	arc.GetCenter(cen);

	if (arc.GetCenter(cen))
	{
		CPoint3D beg = begPt;
		CPoint3D med = midPt;
		CPoint3D end = endPt;

		CVector3D tan1 = arc.GetTangentVector(beg);
		CVector3D tan2 = arc.GetTangentVector(end);

		CPointVect pv1(beg, tan1);
		CPointVect pv2(end, tan2);

		CPoint3D ip;
		bool isIntersect = pv1.Intersect(ip, pv2);

		double radius = arc.GetRadius();

		RebarVertexP vex;
		vex = &(curve.PopVertices()).NewElement();
		vex->SetIP(beg);
		if (curve.PopVertices().GetSize() == 1)
		{
			vex->SetType(RebarVertex::kStart);      // first IP
		}
		else
		{
			vex->SetType(RebarVertex::kIP);
		}
		vers.Add(vex);
		CPoint3D mid = (beg + end) / 2.0;
		CVector3D midVec(cen, mid);
		midVec.Normalize();

		if (isIntersect)
		{
			mid = cen + midVec * radius;

			// it can be on the other size
			CPoint3D mid1 = cen - midVec * radius;

			double d1 = med.Distance(mid1);
			double d2 = med.Distance(mid);

			if (d1 < d2)
			{
				mid = mid1;
				midVec = -midVec;
				// this is big arc we need 4 ips

				CVector3D midTan = midVec.Perpendicular();
				CPointVect pvm(mid, midTan);

				pv1.Intersect(ip, pvm);
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(ip);
				vex->SetType(RebarVertex::kIP);      // 2nd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				mid1 = (beg + mid) / 2.0;
				midVec = mid1 - cen;
				midVec.Normalize();
				mid1 = cen + midVec * radius;

				vex->SetArcPt(0, beg);
				vex->SetArcPt(1, mid1);
				vex->SetArcPt(2, mid);
				vers.Add(vex);


				pv1.Intersect(ip, pvm);
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(beg);
				vex->SetType(RebarVertex::kIP);      // 3rd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				mid1 = (end + mid) / 2.0;
				midVec = mid1 - cen;
				midVec.Normalize();
				mid1 = cen + midVec * radius;

				vex->SetArcPt(0, mid);
				vex->SetArcPt(1, mid1);
				vex->SetArcPt(2, end);
				vers.Add(vex);
			}
			else
			{
				// this is less than 90 or equal we need 3 ips
				vex = &curve.PopVertices().NewElement();
				vex->SetIP(ip);
				vex->SetType(RebarVertex::kIP);      // 2nd IP
				vex->SetRadius(radius);
				vex->SetCenter(cen);

				vex->SetArcPt(0, beg);
				vex->SetArcPt(1, mid);
				vex->SetArcPt(2, end);
				vers.Add(vex);
			}
		}
		else
		{
			// this is half circle - we need 4 ips
			midVec = arc.GetTangentVector(med);
			midVec.Normalize();
			DPoint3d ptMedTan = midVec;
			ptMedTan.Scale(radius);
			ptMedTan.Add(med);
			DPoint3d ptBegTan = tan1;
			ptBegTan.Scale(radius);
			ptBegTan.Add(beg);
			mdlVec_intersect(ip, &DSegment3d::From(beg, ptBegTan), &DSegment3d::From(med, ptMedTan));
			// 			EditElementHandle eeh;
			// 			LineHandler::CreateLineElement(eeh, NULL, DSegment3d::From(beg, ptBegTan), true, *ACTIVEMODEL);
			// 			eeh.AddToModel();
			// 			EditElementHandle eeh1;
			// 			LineHandler::CreateLineElement(eeh1, NULL, DSegment3d::From(med, ptMedTan), true, *ACTIVEMODEL);
			// 			eeh1.AddToModel();
			// 			CPointVect pvm(med, midVec);
			// 			pvm.Intersect(ip, tan1);
			// 			tan1 = ip - beg;
			// 			tan1.Normalize();
			// 
			// 			ip = beg + tan1 * radius;
			// 			midVec = ip - cen;
			// 			midVec.Normalize();
			//			midVec = ip - cen;
			mid = cen + tan1 * radius;
			DEllipse3d circle = DEllipse3d::FromCenterRadiusXY(cen, radius);

			double angle_start = circle.PointToAngle(beg);
			double angle_mid = circle.PointToAngle(mid);

			double angle = (angle_start + angle_mid) / 2;
			// 			if (angle < angle_mid)
			// 				angle += _PI;
			CPoint3D mid1;
			circle.Evaluate(&mid1, 0, angle);

			//			CPoint3D mid1 = cen + midVec;

			vex = &curve.PopVertices().NewElement();
			vex->SetIP(ip);
			vex->SetType(RebarVertex::kIP);      // 2nd IP
			vex->SetRadius(radius);
			vex->SetCenter(cen);

			vex->SetArcPt(0, beg);
			vex->SetArcPt(1, mid1);
			vex->SetArcPt(2, mid);
			vers.Add(vex);
			// 			ip = end + tan1 * radius; // tan1 and tan2 parallel but tan1 has now the correct direction, do not change to tan2
			// 			midVec = ip - cen;
			// 			midVec.Normalize();
			// 			mid1 = cen + midVec * radius;

			//			midVec.Negate();
			DPoint3d ptEndTan = tan2;
			ptEndTan.Scale(radius);
			ptEndTan.Add(end);
			mdlVec_intersect(ip, &DSegment3d::From(end, ptEndTan), &DSegment3d::From(med, ptMedTan));
			// 			midVec = ip - cen;
			// 			mid1 = cen + midVec;

			// 			EditElementHandle eeh2;
			// 			LineHandler::CreateLineElement(eeh2, NULL, DSegment3d::From(end, ptEndTan), true, *ACTIVEMODEL);
			// 			eeh2.AddToModel();
			double angle_end = circle.PointToAngle(end);

			angle = (angle_end + angle_mid) / 2;
			// 			if (angle < angle_end)
			// 				angle += _PI;

			circle.Evaluate(&mid1, 0, angle);

			vex = &curve.PopVertices().NewElement();
			vex->SetIP(ip);
			vex->SetType(RebarVertex::kIP);      // 3rd IP
			vex->SetRadius(radius);
			vex->SetCenter(cen);

			vex->SetArcPt(0, mid);
			vex->SetArcPt(1, mid1);
			vex->SetArcPt(2, end);
			vers.Add(vex);
		}

		vex = &curve.PopVertices().NewElement();
		vex->SetIP(end);
		vex->SetType(RebarVertex::kIP);      // last IP
		vers.Add(vex);
		ret = true;
	}

	return ret;
}

void CustomRebar::movePoint(DPoint3d vec, DPoint3d& movePt, double disLen, bool bFlag)
{
	vec.Normalize();
	if (!bFlag)
	{
		vec.Negate();
	}
	vec.ScaleToLength(disLen);
	movePt.Add(vec);
}

/*void CustomRebar::Create(RebarSetR rebarSet)
{
	DgnModelRefP modelRef = rebarSet.GetModelRef();
	BrString strRebarSize = m_CustomizeRebarInfo.rebarSize;
	if (strRebarSize.Find(L"mm") != WString::npos)
	{
		strRebarSize.Replace(L"mm", L"");
	}

	strcpy(m_CustomizeRebarInfo.rebarSize, CT2A(strRebarSize));
	GetDiameterAddType(m_CustomizeRebarInfo.rebarSize, m_CustomizeRebarInfo.rebarType);//附加型号

	std::vector<PIT::PITRebarCurve> vecRebarCurve;
	double uor_per_mm = modelRef->GetModelInfoCP()->GetUorPerMeter() / 1000.0;
	double dim = RebarCode::GetBarDiameter(strRebarSize, modelRef);
	double bendRadius = RebarCode::GetPinRadius(strRebarSize, modelRef, false);

	vector <vector<pointInfo>>vctCustom;

	vctCustom.push_back(m_vecRebarPts);
	pointInfo CustomTemp;

	double spacing = m_CustomizeRebarInfo.rebarSpacing * uor_per_mm;
	CVector3D vec;
	std::string strDir(m_CustomizeRebarInfo.rebarArrayDir);
	if (strDir == "X")
	{
		vec = CVector3D::kXaxis;
	}
	else if (strDir == "Y")
	{
		vec = CVector3D::kYaxis;
	}
	else
	{
		vec = CVector3D::kZaxis;
	}
	vector<pointInfo> vctTemp;
	for (int a = 0;a < m_CustomizeRebarInfo.rebarArrayNum - 1;a++)
	{
		if (a == 0)
		{
			for (int b = 0; b < m_vecRebarPts.size(); b++)
			{
				CustomTemp.curType = m_vecRebarPts[b].curType;
				CustomTemp.ptCenter = m_vecRebarPts[b].ptCenter;
				CustomTemp.ptEnd = m_vecRebarPts[b].ptEnd;
				CustomTemp.ptStr = m_vecRebarPts[b].ptStr;

				movePoint(vec, CustomTemp.ptStr, spacing);
				movePoint(vec, CustomTemp.ptEnd, spacing);
				movePoint(vec, CustomTemp.ptCenter, spacing);
				vctTemp.push_back(CustomTemp);
			}
		}
		else
		{
	
			vector<pointInfo> vctTTemp;
			vctTTemp.insert(vctTTemp.begin(), vctTemp.begin(), vctTemp.end());
			vctTemp.clear();

			for (int b = 0; b < vctTTemp.size(); b++)
			{
				CustomTemp = vctTTemp.at(b);

				movePoint(vec, CustomTemp.ptStr, spacing);
				movePoint(vec, CustomTemp.ptEnd, spacing);
				movePoint(vec, CustomTemp.ptCenter, spacing);
				vctTemp.push_back(CustomTemp);
			}
		}

		vctCustom.push_back(vctTemp);
	}

	for (auto it = vctCustom.begin();it!= vctCustom.end();it++)
	{
		PIT::PITRebarCurve m_Curve;
		if (m_linestyle == 0)//全为直线
		{
			vector<MSElementDescrP> descrs;
			for (int j = 0; j < (*it).size(); j++)//存储线上所有不重复的点
			{
				if ((*it)[j].curType == ICurvePrimitive::CurvePrimitiveType::CURVE_PRIMITIVE_TYPE_Line)
				{
					EditElementHandle lineeeh;
					LineHandler::CreateLineElement(lineeeh, nullptr, DSegment3d::From((*it)[j].ptStr, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP linedescr = lineeeh.ExtractElementDescr();
					descrs.push_back(linedescr);
				}
				else
				{
					EditElementHandle arceeh;
					ArcHandler::CreateArcElement(arceeh, nullptr, DEllipse3d::FromPointsOnArc((*it)[j].ptStr, (*it)[j].ptMid, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP arcdescr = arceeh.ExtractElementDescr();
					descrs.push_back(arcdescr);
				}
			}
			/*RebarVertices&  vers = m_Curve.PopVertices();
			ArcRebarTool::CalculateArcOrLineRebarVertexs(vers, descrs, bendRadius);
			for (MSElementDescrP tmpdescr : descrs)
			{
				mdlElmdscr_freeAll(&tmpdescr);
				tmpdescr = NULL;
			}#1#
			RebarVertices  vers;
			bvector<DPoint3d> allPts;
			for (int i = 0; i < (*it).size(); i++)
			{
				if (allPts.size() == 0)
				{
					allPts.push_back((*it)[i].ptStr);
					allPts.push_back((*it)[i].ptEnd);
				}
				else
				{
					allPts.push_back((*it)[i].ptEnd);
				}
			}
			GetRebarVerticesFromPoints(vers, allPts, bendRadius);
			m_Curve.SetVertices(vers);
		}
		else if (m_linestyle == 1)//全为弧线
		{
			vector<MSElementDescrP> descrs;
			for (int j = 0; j < (*it).size(); j++)//存储线上所有不重复的点
			{
				if ((*it)[j].curType == ICurvePrimitive::CurvePrimitiveType::CURVE_PRIMITIVE_TYPE_Line)
				{
					EditElementHandle lineeeh;
					LineHandler::CreateLineElement(lineeeh, nullptr, DSegment3d::From((*it)[j].ptStr, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP linedescr = lineeeh.ExtractElementDescr();
					descrs.push_back(linedescr);
				}
				else
				{
					EditElementHandle arceeh;
					ArcHandler::CreateArcElement(arceeh, nullptr, DEllipse3d::FromPointsOnArc((*it)[j].ptStr, (*it)[j].ptMid, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP arcdescr = arceeh.ExtractElementDescr();
					descrs.push_back(arcdescr);
				}
			}
			RebarVertices&  vers = m_Curve.PopVertices();
			ArcRebarTool::GetArcOrLineRebarVertices(vers, descrs, bendRadius);
			for (MSElementDescrP tmpdescr : descrs)
			{
				mdlElmdscr_freeAll(&tmpdescr);
				tmpdescr = NULL;
			}
		}
		else
		{
			vector<MSElementDescrP> descrs;
			for (int j = 0; j < (*it).size(); j++)//存储线上所有不重复的点
			{
				if ((*it)[j].curType == ICurvePrimitive::CurvePrimitiveType::CURVE_PRIMITIVE_TYPE_Line)
				{
					EditElementHandle lineeeh;
					LineHandler::CreateLineElement(lineeeh, nullptr, DSegment3d::From((*it)[j].ptStr, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP linedescr = lineeeh.ExtractElementDescr();
					descrs.push_back(linedescr);
				}
				else
				{
					EditElementHandle arceeh;
					ArcHandler::CreateArcElement(arceeh, nullptr, DEllipse3d::FromPointsOnArc((*it)[j].ptStr, (*it)[j].ptMid, (*it)[j].ptEnd), true, *ACTIVEMODEL);
					MSElementDescrP arcdescr = arceeh.ExtractElementDescr();
					descrs.push_back(arcdescr);
				}
			}
			RebarVertices&  vers = m_Curve.PopVertices();
			ArcRebarTool::GetArcOrLineRebarVertices(vers,descrs,bendRadius);
			for (MSElementDescrP tmpdescr:descrs)
			{
				mdlElmdscr_freeAll(&tmpdescr);
				tmpdescr = NULL;
			}
		}

		vecRebarCurve.push_back(m_Curve);
	}

	int index = 0;
	int rebarNum = (int)vecRebarCurve.size();
	for (PIT::PITRebarCurve rebarCurve : vecRebarCurve)
	{
		if (rebarCurve.PopVertices().GetSize()==0)
		{
			continue;
		}
		RebarVertexP vex2;
		vex2 = rebarCurve.PopVertices().GetAt(rebarCurve.PopVertices().GetSize() - 1);
		vex2->SetType(RebarVertex::kEnd);

		RebarSymbology symb;
		string str(strRebarSize);
		char ccolar[20] = { 0 };
		strcpy(ccolar, str.c_str());
		SetRebarColorBySize(ccolar, symb);
		symb.SetRebarLevel(TEXT_MAIN_REBAR);

		RebarElementP rebarElement = rebarSet.AssignRebarElement(index, rebarNum, symb, modelRef);
		if (nullptr != rebarElement)
		{
			RebarShapeData shape;
			shape.SetSizeKey((LPCTSTR)strRebarSize);
			shape.SetIsStirrup(false);
			shape.SetLength(rebarCurve.GetLength() / uor_per_mm);
			RebarEndType endType;
			endType.SetType(RebarEndType::kNone);
			RebarEndTypes endTypes = { endType,endType };
			rebarElement->Update(rebarCurve, dim, endTypes, shape, modelRef, false);

			ElementId eleid = rebarElement->GetElementId();
			EditElementHandle tmprebar(eleid, ACTIVEMODEL);
			string Stype(m_CustomizeRebarInfo.rebarbsType);
			string Level(m_CustomizeRebarInfo.rebarLevel);
			ElementRefP oldref = tmprebar.GetElementRef();
			SetRebarLevelItemTypeValue(tmprebar, Level, m_CustomizeRebarInfo.rebarType, Stype, modelRef);
			SetRebarHideData(tmprebar, spacing/uor_per_mm, ACTIVEMODEL);
			tmprebar.ReplaceInModel(oldref);
			m_RebarID.push_back(eleid);

		}
		index++;
	}
}*/

void CustomRebar::Create(RebarSetR rebarSet)
{
	// 获取当前模型引用并初始化钢筋尺寸处理
	DgnModelRefP modelRef = rebarSet.GetModelRef();
	BrString strRebarSize = m_CustomizeRebarInfo.rebarSize;
	if (strRebarSize.Find(L"mm") != WString::npos)
	{
		strRebarSize.Replace(L"mm", L"");
	}

	strcpy(m_CustomizeRebarInfo.rebarSize, CT2A(strRebarSize));
	GetDiameterAddType(m_CustomizeRebarInfo.rebarSize, m_CustomizeRebarInfo.rebarType);//附加型号

	std::vector<PIT::PITRebarCurve> vecRebarCurve;
	double uor_per_mm = modelRef->GetModelInfoCP()->GetUorPerMeter() / 1000.0;
	double dim = RebarCode::GetBarDiameter(strRebarSize, modelRef);
	double bendRadius = RebarCode::GetPinRadius(strRebarSize, modelRef, false);

	// 生成排列点
	std::vector<std::vector<pointInfo>> customPoints{ m_vecRebarPts };
	double spacing = m_CustomizeRebarInfo.rebarSpacing * uor_per_mm;
	std::string dir = m_CustomizeRebarInfo.rebarArrayDir;
	CVector3D vec = (dir == "X") ? CVector3D::kXaxis :
		(dir == "Y") ? CVector3D::kYaxis : CVector3D::kZaxis;

	// 初始化点集并生成钢筋排列点
	// 每一根钢筋曲线位置通过自定义曲线点逐渐按照设定的方向以及间距偏移得到
	std::vector<pointInfo> tempPoints;
	for (int a = 0; a < m_CustomizeRebarInfo.rebarArrayNum - 1; a++)
	{
		tempPoints.clear();
		const auto& basePoints = (a == 0) ? m_vecRebarPts : customPoints.back();
		for (const auto& point : basePoints)
		{
			pointInfo newPoint = point;
			movePoint(vec, newPoint.ptStr, spacing);
			movePoint(vec, newPoint.ptEnd, spacing);
			movePoint(vec, newPoint.ptCenter, spacing);
			tempPoints.push_back(newPoint);
		}
		customPoints.push_back(tempPoints);
	}

	// 遍历每个点集，创建相应的钢筋曲线
	std::vector<PIT::PITRebarCurve> rebarCurves;
	for (const auto& points : customPoints)
	{
		PIT::PITRebarCurve curve;
		std::vector<MSElementDescrP> descrs;
		for (const auto& point : points)
		{
			EditElementHandle eeh;
			MSElementDescrP descr;
			if (point.curType == ICurvePrimitive::CurvePrimitiveType::CURVE_PRIMITIVE_TYPE_Line)
			{
				LineHandler::CreateLineElement(eeh, nullptr, DSegment3d::From(point.ptStr, point.ptEnd), true, *ACTIVEMODEL);
			}
			else
			{
				ArcHandler::CreateArcElement(eeh, nullptr, DEllipse3d::FromPointsOnArc(point.ptStr, point.ptMid, point.ptEnd), true, *ACTIVEMODEL);
			}
			descr = eeh.ExtractElementDescr();
			descrs.push_back(descr);
		}

		// 生成钢筋顶点数据
		RebarVertices& vers = curve.PopVertices();
		if (m_linestyle == 0) // 全直线
		{
			bvector<DPoint3d> allPts;
			for (size_t i = 0; i < points.size(); i++)
			{
				if (allPts.empty())
				{
					allPts.push_back(points[i].ptStr);
				}
				allPts.push_back(points[i].ptEnd);
			}
			GetRebarVerticesFromPoints(vers, allPts, bendRadius);
		}
		else // 弧线或混合
		{
			ArcRebarTool::GetArcOrLineRebarVertices(vers, descrs, bendRadius);
		}

		for (auto descr : descrs)
		{
			mdlElmdscr_freeAll(&descr);
		}
		rebarCurves.push_back(curve);
	}

	// 遍历钢筋曲线，配置并保存钢筋元素
	int index = 0;
	for (auto& rebarCurve : rebarCurves)
	{
		if (rebarCurve.PopVertices().GetSize() == 0) continue;

		// 设置曲线末端顶点类型
		RebarVertexP lastVertex = rebarCurve.PopVertices().GetAt(rebarCurve.PopVertices().GetSize() - 1);
		lastVertex->SetType(RebarVertex::kEnd);

		RebarSymbology symb;
		SetRebarColorBySize(strRebarSize, symb);
		symb.SetRebarLevel(TEXT_MAIN_REBAR);

		// 为钢筋集分配新元素
		RebarElementP rebarElement = rebarSet.AssignRebarElement(index++, rebarCurves.size(), symb, modelRef);
		if (!rebarElement) continue;

		// 设置钢筋形状数据
		RebarShapeData shape;
		shape.SetSizeKey((LPCTSTR)strRebarSize);
		shape.SetIsStirrup(false);
		shape.SetLength(rebarCurve.GetLength() / uor_per_mm);
		RebarEndType endType;
		endType.SetType(RebarEndType::kNone);
		RebarEndTypes endTypes = { endType, endType };
		rebarElement->Update(rebarCurve, dim, endTypes, shape, modelRef, false);

		// 获取元素ID并进行进一步配置
		ElementId eleid = rebarElement->GetElementId();
		EditElementHandle tmprebar(eleid, ACTIVEMODEL);
		ElementRefP oldref = tmprebar.GetElementRef();
		SetRebarLevelItemTypeValue(tmprebar, m_CustomizeRebarInfo.rebarLevel, m_CustomizeRebarInfo.rebarType, m_CustomizeRebarInfo.rebarbsType, modelRef);
		SetRebarHideData(tmprebar, spacing / uor_per_mm, ACTIVEMODEL);
		tmprebar.ReplaceInModel(oldref);
		m_RebarID.push_back(eleid);
	}
}

RebarSetTag * CustomRebarAssembly::MakeRebar(ElementId rebarSetId, DgnModelRefP modelRef)
{
	if (modelRef == nullptr)
	{
		return nullptr;
	}

	RebarSetP rebarSet = RebarSet::Fetch(rebarSetId, modelRef);
	rebarSet->SetRebarDisplayMode(RebarDisplayMode::kRebarCylinderMode);
	rebarSet->StartUpdate(modelRef);

	if (rebarSet == nullptr)
	{
		return nullptr;
	}

	for (const auto& customRebar : m_vecCustomRebar)
	{
		customRebar->Create(*rebarSet);
	}

	RebarSetTag *tag = new RebarSetTag;
	tag->SetRset(rebarSet);
	tag->SetIsStirrup(false);
	return tag;
}


