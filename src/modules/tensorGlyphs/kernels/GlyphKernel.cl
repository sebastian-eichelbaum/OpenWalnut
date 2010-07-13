inline bool isInsideGlyph(const float4 pos,const float* coeffs,const uint numOfCoeffs)
{
	return true;
}

__kernel void renderGlyphs(float originX,float originY,float originZ,
						   float llX,float llY,float llZ,
						   float xx,float xy,float xz,
						   float yx,float yy,float yz,
						   float near,float far,
						   uint width,uint height,
						   __global float* tensorData,
						   uint numOfTensors,uint numOfCoeffs,
						   __write_only image2d_t color,__write_only image2d_t depth)
{
	if (get_global_id(0) >= width) return;
	if (get_global_id(1) >= height) return;

	float4 direction = (float4)(llX,llY,llZ,0.0f);

	float ratioX = (float)get_global_id(0) / (float)width;
	float ratioY = (float)get_global_id(1) / (float)height;

	float4 initialPoint = (float4)
	(
		originX + ratioX * xx + ratioY * yx,
		originY + ratioX * xy + ratioY * yy,
		originZ + ratioX * xz + ratioY * yz,
		0.0f
	);

	// etc. pp.

	write_imagef(color,(int2)(get_global_id(0),get_global_id(1)),(float4)(1.0,0.0,0.0,1.0));
	write_imagef(depth,(int2)(get_global_id(0),get_global_id(1)),(float4)(0.0,0.0,0.0,0.5));
}