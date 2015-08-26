#include "base/search.h"

namespace base
{
}

#ifdef _SEARCH_MAIN_TEST_
#include <assert.h>

int main(int argc, char *argv[])
{
    using namespace base;

    int arr_odd[] = {40, 35, 30, 25, 20, 15, 10};
    int pos = 0;
    int test_arr_odd[] = {43, 40, 37, 35, 32, 30, 28, 25, 21, 20, 16, 15, 13, 10, 2 };
    for (int i = 0; i < (int)(sizeof(test_arr_odd)/sizeof(test_arr_odd[0])); ++i)
    {
        Code ret = BinarySearch(arr_odd, sizeof(arr_odd)/sizeof(arr_odd[0]),
                                test_arr_odd[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_odd[i], pos, arr_odd[pos]);
    }
    fprintf(stderr, "\n");

    int arr_even[] = {40, 35, 30, 25, 20, 15};
    int test_arr_even[] = {44, 40, 38, 35, 33, 30, 26, 25, 24, 20, 18, 15, 11};
    for (int i = 0; i < (int)(sizeof(test_arr_even)/sizeof(test_arr_even[0])); ++i)
    {
        Code ret = BinarySearch(arr_even, sizeof(arr_even)/sizeof(arr_even[0]),
                                test_arr_even[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_even[i], pos, arr_even[pos]);
    }

    return 0;
}
#endif
