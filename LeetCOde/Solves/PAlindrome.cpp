// class Solution {
// public:
//     bool isPalindrome(int x) {
//         std::string x_ = std::to_string(x);
//         std::string x1 = x_;
//         std::reverse(x1.begin(),x1.end());
//         return x_ == x1;
//     }
// };





//  Математическое решение 


// class Solution {
// public:
//     bool isPalindrome(int x) {
//         if (x<0 || (x%10 == 0 && x!=0)) return false;
//         long long rev = 0;
//         while (x>rev) {
//             rev = (rev*10) + (x%10);
//             x/=10;
//         }
//         return (rev == x || x == rev/10);
//     }
// };