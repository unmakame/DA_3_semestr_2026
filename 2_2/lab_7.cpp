#include <iostream>
#include <vector>

using namespace std;

class NumberGame {
private:
    using ll = long long;
    int n;
    vector<ll> dp;
    vector<int> p;

public:
    NumberGame(int n_) : n(n_) {
        dp.resize(n + 1, 0);
        p.resize(n + 1, 0);
    }

    void solve() {
        for (int i = 2; i <= n; i++) {
            dp[i] = dp[i - 1] + i;
            p[i] = i - 1;

            if (i % 2 == 0 && dp[i / 2] + i < dp[i]) {
                dp[i] = dp[i / 2] + i;
                p[i] = i / 2;
            }

            if (i % 3 == 0 && dp[i / 3] + i < dp[i]) {
                dp[i] = dp[i / 3] + i;
                p[i] = i / 3;
            }
        }
    }

    void print() const {
        cout << dp[n] << '\n';
        vector<int> ans;
        int x = n;

        while (x > 1) {
            if (p[x] == x - 1) {
                ans.push_back(1);
            } else if (x % 2 == 0 && p[x] == x / 2) {
                ans.push_back(2);
            } else {
                ans.push_back(3);
            }

            x = p[x];
        }

        for (int i = 0; i < (int)ans.size(); i++) {
            if (i > 0) cout << ' ';

            if (ans[i] == 1) cout << "-1";
            else if (ans[i] == 2) cout << "/2";
            else cout << "/3";
        }
        cout << '\n';
    }
};

int main(){
    int n; cin >> n;
    NumberGame game(n);
    game.solve();
    game.print();

    return 0;
}