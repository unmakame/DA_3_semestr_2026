// /**
//  * Definition for singly-linked list.
//  * struct ListNode {
//  *     int val;
//  *     ListNode *next;
//  *     ListNode() : val(0), next(nullptr) {}
//  *     ListNode(int x) : val(x), next(nullptr) {}
//  *     ListNode(int x, ListNode *next) : val(x), next(next) {}
//  * };
//  */
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode *dumb = new ListNode(0);
        dumb->next = head;

        ListNode *first = dumb;
        ListNode *second = dumb;

        for(int i = 0; i <= n;++i){
            first = first->next;
        }

        while(first != nullptr){
            first = first->next;
            second = second->next;
        }

        ListNode *temp = second->next;
        second->next = second->next->next;
        delete temp;
        
        ListNode *newHead = dumb->next;
        delete dumb;
        return newHead;
    }
};