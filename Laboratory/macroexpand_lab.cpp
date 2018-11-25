

# if 0
/*from goxel/src/mesh.c*/
#define BLOCK_SIZE 16
#define N BLOCK_SIZE

#define BLOCK_ITER(x, y, z) \
    for (z = 0; z < N; z++) \
        for (y = 0; y < N; y++) \
            for (x = 0; x < N; x++)

#define DATA_AT(d, x, y, z) (d->voxels[x + y * N + z * N * N])
#define BLOCK_AT(c, x, y, z) (DATA_AT(c->data, x, y, z))

static bool block_is_empty(const block_t *block, bool fast)
{
	int x, y, z;
	if (!block) return true;
	if (block->data->id == 0) return true;
	if (fast) return false;

	BLOCK_ITER(x, y, z) {
		if (BLOCK_AT(block, x, y, z)[3]) return false;
	}
	return true;
}
#endif

# if 0
////////////////////////////////////////////////////////////////////////////////
/*expand and print*/
#define MIN(x,y) (x<y?x:y)
#define TO_STRING(x) TO_STRING1(x)
#define TO_STRING1(x) #x
int main() {
	const char *str = TO_STRING(MIN(1, 2));
	printf(str);
	return 0;
}

void Foo()
#endif
