\#include "${CLASS_NAME}.h"

#set ($CLASS_NAME_SNAKE_CASE = $CLASS_NAME.replaceAll("([a-z])([A-Z])", "$1_$2").toLowerCase())
namespace ${CLASS_NAME_SNAKE_CASE} {
    ${CLASS_NAME}::${CLASS_NAME}(): Entity() {}
}