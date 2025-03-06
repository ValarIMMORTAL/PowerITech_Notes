# Bentley常用API记录

## mdlElmdscr_extractEndPoints

用于提取几何元素的起始点、终点及其切线。此函数适用于多种元素类型，包括线、线串、弧线、曲线、B-spline 曲线、多线、复合链条和复合形状等。

### 函数参数说明

- **`startP`**: 输出参数，表示曲线的起始点。可以传入 `NULL` 如果不需要此值。
- **`startTangentP`**: 输出参数，表示起始点的切线。可以传入 `NULL` 如果不需要此值。
- **`endP`**: 输出参数，表示曲线的终点。可以传入 `NULL` 如果不需要此值。
- **`endTangentP`**: 输出参数，表示终点的切线。可以传入 `NULL` 如果不需要此值。
- **`edP`**: 输入参数，表示要操作的元素描述符（`MSElementDescrCP`）。
- **`modelRef`**: 输入参数，表示模型引用，仅在输入元素为多线时使用。

### 功能说明

该函数返回给定元素的起始点和终点信息。它会通过计算来提取这些信息，包括曲线的起始和终止点及其切线，适用于多种几何元素类型，如线、线串、弧线、B-spline 曲线等。

### 返回值

- **`SUCCESS`**: 如果函数成功返回了起始点、终点和切线信息。
- **`MDLERR_BADTYPE`**: 如果提供的元素类型无效，不能提取相应的起始点、终点和切线。



## vec.DifferenceOf

void DifferenceOf  (DPoint3dCR target, DPoint3dCR base);

通过起止点确定向量方向



## ptStart.SumOf

void SumOf (DPoint3dCR origin, DVec3dCR vector, double scale);

向某个方向移动点，注意它会用向量长度，有必要可以先把向量归零







mdlElmdscr_extractNormal	寻找元素的法向量
mdlElmdscr_computeRange	计算指定元素描述符中元素的范围
mdlElmdscr_add	元素加入显示模型
mdlElmdscr_convertTo2D	3D元素描述符转换为2D元素描述符
mdlIntersect_allBetweenElms 	获取两个元素之间的所有交点。
mdlElmdscr_extractEndPoints 	返回起始点和结束点
mdlVec_distance	计算两点之间的距离。

mdlElmdscr_duplicate	创建元素副本
mdlVec_addPoint	计算两个点或向量的和
mdlLinear_getPointCount	获取指定元素中的顶点数。
mdlElmdscr_freeAll	释放descrip
mdlElmdscr_addByModelRef	元素加入指定显示模型
mdlMeasure_linearProperties 	获取元素的基本线性属性
mdlModelRef_activateAndDisplay 	激活并显示指定的modelRef
mdlVec_projectPointToLine 	计算一个点在由两点定义的直线上的投影

mdlLinear_extract	从线性元素中提取坐标数组
mdlIntersect_allBetweenExtendedElms	获取两个元素无线扩展后之间的所有交点。
DSegment3d::IntersectXY	求两线段（无线延申）的交点
mdlVec_intersect	求两线段的交点
CurveCurve::IntersectionsXY	求交
mdlVec_projectPointToPlane 投影点到面
mdlArc_extract 
求向量的垂直向量：crossproduct（叉乘）
mdlElmdscr_reverseNormal	翻转表面、实体或封闭元件的方向
mdlElmdscr_reverse		翻转线、线串、形状、弧线、曲线、b样条曲线、复杂链和复杂形状的方向
LocateSubEntityTool		在元素几何上挑选子实体(面/边/顶点)的工具基类
	_OnElementModify 完成时会触发 _OnModifyComplete
	_WantAdditionalLocate	开启ctrl多选
	_WantAdditionalSubEntities	开启多个实体元素多选
	GetAcceptedSubEntities	获取当前实体选中的元素
mdlVec_dotProduct	向量的点积d（一个向量在另一个向量方向上投影的长度），用来获取向量的夹角α，d>0时，α<90°；d<0时，α>90°；d=0，阿尔法=90°。
if (fabs(fabs(tmpVec.DotProduct(dimVec)) - 1) < 1e-6) //判断平行
旋转：
	

```C++
mdlRMatrix_getIdentity(&rMatrix);	构造旋转矩阵
	mdlRMatrix_fromVectorToVector(&rMatrix, &firstface.faceVec, &vecz);	使用面的法向量（参数2）旋转到Z方向（参数3）初始化旋转矩阵
	mdlTMatrix_fromRMatrix(&Trans, &rMatrix);	定义坐标系和仿射变换的3x4矩阵
	mdlTMatrix_setOrigin(&Trans, &ptStart);	设置转换固定点
	mdlElmdscr_transform(&firstface.DownProjectface.faceDescr, &Trans);	将面通过矩阵旋转
{8*XC,   YC/2,30*XC, 0}	{左边距，右边距，宽， 高}（0自适应）
DetailingSymbolManager::CalculateElementRange 	通过eeh获取range

mdlVec_projectPointToPlaneInView 	投影点到面
ReachableElementCollection eleCollection = ACTIVEMODEL->GetReachableElements();	//获取dgnmodel中的所有元素，包括参考
	for each (ElementHandle elem in eleCollection) 	//遍历
mdlElmdscr_setVisible(eeh.GetElementDescrP(), true);	//显示或隐藏元素	
mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, std::to_wstring(num).c_str(), MessageBoxIconType::Information); //弹窗
```

x方向旋转到某个方向：
	

```C++
RotMatrix rot;
	rot.InitIdentity();
	DVec3d vecx = DVec3d::UnitX();
	mdlRMatrix_fromVectorToVector(&rot, &vecx, &gradientVec);
	Transform	tMatrix;
	tMatrix = Transform::From(rot, textPt);
	mdlTMatrix_setOrigin(&tMatrix, &textPt);
	TransformInfo tmatrixinfo(tMatrix);
	Texteeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(Texteeh, tmatrixinfo);
```

构造投影矩阵

```C++
DPoint3d zeropt = DPoint3d::From(0, 0, 0);
	DPoint3d tmpvecZ = DPoint3d::From(0, 0, 1);
	Transform tran;			//构造投影矩阵
	mdlTMatrix_computeFlattenTransform(&tran, &zero
```

平移

```
mdlCurrTrans_begin();
	Transform tMatrix;
	mdlTMatrix_getIdentity(&tMatrix);
	mdlTMatrix_setTranslation(&tMatrix, &m_moveDir);
	EditElementHandle eeh(it, it->GetDgnModelP());
	MSElementDescrP newLine = eeh.GetElementDescrP();
	mdlElmdscr_transform(&newLine, &tMatrix);
	mdlCurrTrans_end();
```


旋转平移文本块

```
//旋转
RotMatrix rot;
rot.InitIdentity();

DVec3d vecx = DVec3d::UnitX();
mdlRMatrix_fromVectorToVector(&rot, &vecx, &textVec);
Transform	rotTMatrix;
rotTMatrix = Transform::From(rot, dimPt);
mdlTMatrix_setOrigin(&rotTMatrix, &dimPt);
//平移
Transform tranMatrix;
mdlTMatrix_getIdentity(&tranMatrix);
DPoint3d move = textPt - dimPt;
mdlTMatrix_setTranslation(&tranMatrix, &move);
//合并运用
Transform transform;
mdlTMatrix_multiply(&transform, &tranMatrix, &rotTMatrix);
TransformInfo tmatrixinfo(transform);
Texteeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(Texteeh, tmatrixinfo);
```

缩放矩阵
	RotMatrix matrix;
	mdlRMatrix_getIdentity(&matrix);
	mdlRMatrix_scale(&matrix, lenth, width, height);//长宽高的比例
	或
	Transform matric;
	mdlTMatrix_getIdentity(&matrix);
	mdlTMatrix_scale(&matrix, lenth, width, height);

DPoint3d origin = DPoint3d::FromZero();

RotMatrix rotM;
rotM.InitIdentity();
EditElementHandle cellEeh;
NormalCellHeaderHandler::CreateCellElement(cellEeh, L"坡度标注", origin, rotM, modelref->Is3d(), *modelref);
NormalCellHeaderHandler::AddChildElement(cellEeh, addlineEeh);
NormalCellHeaderHandler::AddChildComplete(cellEeh);

//复杂元素
EditElementHandle eeh;
ChainHeaderHandler::CreateChainHeaderElement(eeh, nullptr, true, false, *modelref);

bstatus = ComplexShapeHandler::AddComponentElement(eeh, eehArc1);
bstatus = ComplexShapeHandler::AddComponentComplete(eeh）

//判断点与面的关系
EditElementHandle shapeEeh; //这个必须是封闭面，创建时若有isClosed参数设为true就是封闭面
CurveVectorPtr curvePtr = ICurvePathQuery::ElementToCurveVector(shapeEeh); //转换面
DPoint3d strPt; //点
CurveVector::InOutClassification pos1 = curvePtr->PointInOnOutXY(strPt); //点和面的关系

//获取rgb对应的颜色
UInt32 fillColor;
RgbColorDef rgb{ 174, 174, 174 };
mdlColor_rawColorFromRGBColor(&fillColor, &rgb, dgnModelPtr);

EB：楼梯	
PB/NB：梁柱	
VB：墙	
DB：板
MB：设备基础

出图工具函数：
	SolidBoolOperation	实体求交、并、差
	
查看元素类型的命令
	analyze element

选择集
	ElementAgenda selectedElement;
	SelectionSetManager::GetManager().BuildAgenda(selectedElement);

mdlDialog_dmsgsPrint(seedFolder.data());