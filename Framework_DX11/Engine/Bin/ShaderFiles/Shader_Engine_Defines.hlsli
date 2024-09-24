
sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

//RS : RasterizerState 채우는 방식?
//BS : Blend State
RasterizerState RS_Default
{
	FillMode = solid;				//면, 와이어 등
	CullMode = back;				//전면, 후변
	FrontCounterClockwise = false; //시계 , 반시계
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
	FillMode = solid;
	CullMode = none;			//전면, 후면 다 보여주기
	FrontCounterClockwise = false;
};

//깊이 버퍼
DepthStencilState DSS_Default
{
	DepthEnable = true; //비교를 한다.
	DepthWriteMask = all; //정점 ?싹 다 기록
	DepthFunc = less_equal; // 작거나 같은걸 그린다
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};


BlendState BS_Default
{
//8개까지 가능해서 배열로 구분 가능
	BlendEnable[0] = false;	
};

BlendState BS_AlphaBlend //기존 알파 블렌드
{
	BlendEnable[0] = true;	

	SrcBlend = Src_Alpha; //기존
	DestBlend = Inv_Src_Alpha; //1-기존
	BlendOp = Add;

	/*

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;

	SrcBlend[1] = one;
	DestBlend[1] = one;
	BlendOp[1] = Add;*/
};

BlendState BS_OnebyOne
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};