#include <vector>
#include <stdexcept>

template<typename Type>
Type safeAt(std::vector<Type>& in, int at)
{
    if(in.size()<=at||at<0){
        throw std::invalid_argument("invalid Index");
    }
    return in[at];
}