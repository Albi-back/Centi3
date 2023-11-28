#include "Constants.hlsl"

float4 PSCore(VertexOut pin, uniform bool gUseTexture) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

	float3 toEyeW = normalize(gEyePosW.xyz - pin.PosW);

	float4 texColor = float4(1, 1, 1, 1);
	if (gUseTexture)
		// Sample texture.
		texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;
	[unroll]
	for (int lIdx = 0; lIdx < MAX_LIGHTS; ++lIdx)
	{
		if (gLights[lIdx].Type == LIGHT_DIR)
		{
			ComputeDirectionalLight(gMaterial, gLights[lIdx], pin.NormalW, toEyeW, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}
		else if (gLights[lIdx].Type == LIGHT_POINT)
		{
			ComputePointLight(gMaterial, gLights[lIdx], pin.PosW, pin.NormalW, toEyeW, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		}
		else if (gLights[lIdx].Type == LIGHT_SPOT)
		{
			ComputeSpotLight(gMaterial, gLights[lIdx], pin.PosW, pin.NormalW, toEyeW, A, D, S);
			ambient += A;
			diffuse += D;
			spec += S;
		} 
	}

	//sum intensities 
	float4 litColor = texColor*(ambient + diffuse) + spec;

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}

