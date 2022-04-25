R"(
int wrap(const int a, const int b)
{
    return (a + b) % b;
}

__kernel void process(__global unsigned char *world, int rows, int cols, __global unsigned char *result)
{
    //Get the index of the work-item
    int i = get_global_id(0);
    int x = i % cols;
    int y = i / cols;
    int n = 0;

	result[i] = 0;

	n += world[wrap(x - 1, cols) + y * cols]; 
    n += world[wrap(x + 1, cols) + y * cols];
	n += world[x + wrap(y - 1, rows) * cols];
    n += world[x + wrap(y + 1, rows) * cols];
    n += world[wrap(x + 1, cols) + wrap(y + 1, rows) * cols];
    n += world[wrap(x + 1, cols) + wrap(y - 1, rows) * cols];
    n += world[wrap(x - 1, cols) + wrap(y + 1, rows) * cols];
    n += world[wrap(x - 1, cols) + wrap(y - 1, rows) * cols];

    if (world[i] == 0) {
		if (n == 3 || n == 6)
		    result[i] = 1;
    } else {
		if (n == 2 || n == 3)
		    result[i] = 1;
    }
}
)"