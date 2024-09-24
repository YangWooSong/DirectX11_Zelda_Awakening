
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

//RS : RasterizerState ä��� ���?
//BS : Blend State
RasterizerState RS_Default
{
	FillMode = solid;				//��, ���̾� ��
	CullMode = back;				//����, �ĺ�
	FrontCounterClockwise = false; //�ð� , �ݽð�
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
	CullMode = none;			//����, �ĸ� �� �����ֱ�
	FrontCounterClockwise = false;
};

//���� ����
DepthStencilState DSS_Default
{
	DepthEnable = true; //�񱳸� �Ѵ�.
	DepthWriteMask = all; //���� ?�� �� ���
	DepthFunc = less_equal; // �۰ų� ������ �׸���
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};


BlendState BS_Default
{
//8������ �����ؼ� �迭�� ���� ����
	BlendEnable[0] = false;	
};

BlendState BS_AlphaBlend //���� ���� ����
{
	BlendEnable[0] = true;	

	SrcBlend = Src_Alpha; //����
	DestBlend = Inv_Src_Alpha; //1-����
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