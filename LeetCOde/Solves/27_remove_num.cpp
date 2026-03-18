// class Solution {
// public:
//     int removeElement(vector<int>& nums, int val) {
//         int good_num = 0;
//         for(int i; i < nums.size(); i++){
//             if (nums[i] != val){
//                 nums[good_num] = nums[i];
//                 good_num++;
//             }
//         }
//         return good_num;
//     }
// };