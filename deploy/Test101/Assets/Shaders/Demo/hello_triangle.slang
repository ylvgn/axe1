struct VertexIn
{
    float3 position : POSITION;
    float4 color 	: COLOR;
};

struct PixelIn
{
    float4 position : SV_POSITION;
    float4 color 	: COLOR;
};


[shader("vertex")]
PixelIn vs_main(VertexIn i)
{
    PixelIn o;
    o.position = float4(i.position, 1.0);
    o.color = i.color;
    return o;
}


[shader("fragment")]
float4 ps_main(PixelIn i) : SV_TARGET
{
	//return float4(1,0,0,1);
    return i.color;
}