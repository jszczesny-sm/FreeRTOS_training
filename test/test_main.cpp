#include <unity.h>

void test_invalid_io(void) {
    TEST_ASSERT_EQUAL(1,1);
}

int main()
{

    UNITY_BEGIN();
    RUN_TEST(test_invalid_io);
    UNITY_END(); // stop unit testing

    while(1){}
}
