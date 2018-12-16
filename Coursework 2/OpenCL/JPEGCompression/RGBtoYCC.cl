__kernel void RGBtoYCC(__global float *imgY, __global float *imgC1, __global float *imgC2, __global uchar *src)
{
	// Get id (x)
	int x = get_global_id(0);
	// Get RGB values
	int r = src[x * 3];
	int g = src[x * 3 + 1];
	int b = src[x * 3 + 2];
	imgY[x] = (0.299     * r) + (0.587     * g) + (0.114     * b) - 128.0;
	imgC1[x] = -(0.168736  * r) - (0.331264  * g) + (0.5       * b);
	imgC2[x] = (0.5       * r) - (0.418688  * g) - (0.081312  * b);
}