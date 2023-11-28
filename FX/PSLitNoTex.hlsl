
#include "PSCore.hlsl"

float4 main(VertexOut pin) : SV_Target
{
	return PSCore(pin, false);
}

