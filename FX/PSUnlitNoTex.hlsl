#include "Constants.hlsl"

float4 main(VertexOut pin) : SV_Target
{
	return gMaterial.Diffuse;
}
  
  