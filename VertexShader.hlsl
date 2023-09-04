cbuffer mycBuffer : register(b0)
{
    float xOffset;
    float yOffset;
};

struct VS_INPUT
{
    float2 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float2 tmp = input.inPos;
    input.inPos.x += xOffset;
    input.inPos.y += yOffset;
    output.outPos = float4(input.inPos, 0.0f, 1.0f);
    tmp.x += 0.5;
    tmp.y += 0.5;
    output.outTexCoord = tmp;
    return output;
}