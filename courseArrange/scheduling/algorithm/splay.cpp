#include <algorithm>
using namespace std;

const int N = 130005;
struct Node {
    int key, size;
    bool rvs;
    Node *f, *ch[2];
    void set(int c, Node *x);   //�赱ǰ������(0)/��(1)����Ϊx
    void fix();                 //���������ӵĸ���ָ��ָ���Լ�����ҪΪ��д�������㣬������ȷ
    void pushdown();            //����´�
    void update();              //�Ӷ��Ӵ������Լ�����Ϣ
    void rotate();              //������ת
    void Splay(Node *);         //�ѵ�ǰNode��ת�����������Node���档NodeĬ��Ϊnull��ֱ�ӵ���Splay()����ת����
}statePool[N], *null;           //��ģ��ͳһ��null����NULL
void Node::set(int c, Node *x) {
    ch[c] = x;
    x->f = this;
}
void Node::fix() {
    ch[0]->f = this;
    ch[1]->f = this;
}
void Node::pushdown() {
    if (this == null) return;
    if (rvs) {
        ch[0]->rvs ^= 1;
        ch[1]->rvs ^= 1;
        rvs = 0;
        swap(ch[0], ch[1]);
    }
}
void Node::update() {
    if (this == null) return;
    size = ch[0]->size + ch[1]->size + 1;
}
void Node::rotate() {
    Node *x = f;
    bool o = f->ch[0] == this;
    x->set(!o, ch[o]);
    x->f->set(x->f->ch[1] == x, this);
    set(o, x);
    x->update();
    update();
}
void Node::Splay(Node *goal = null) {
    pushdown();
    for (; f != goal;) {
        f->f->pushdown();
        f->pushdown();
        pushdown();
        if (f->f == goal) {
            rotate();
        } else if ((f->f->ch[0] == f) ^ (f->ch[0] == this)) {
            rotate();
            rotate();
        } else {
            f->rotate();
            rotate();
        }
    }
}
namespace Splay {
    int poolCnt;
    Node *newNode() {
        Node *p = &statePool[poolCnt++];
        p->f = p->ch[0] = p->ch[1] = null;
        p->size = 1;
        p->rvs = 0;
        return p;
    }
    //�������ռ���ĺ��������ȵ���init()
    void init() {
        poolCnt = 0;
        null = newNode();
        null->size = 0;
    }

    //��a[l..r]��ֵ����һ����ȫƽ���Splay tree������������
    template <class T> Node *build(int l, int r, T a[]) {
        if (l > r) return null;
        Node *p = newNode();
        int mid = (l + r) / 2;
        p->key = a[mid];
        if (l < r) {
            p->ch[0] = build(l, mid - 1, a);
            p->ch[1] = build(mid + 1, r, a);
            p->update();
            p->fix();
        }
        return p;
    }
    
    //�������е�i��Ԫ�أ���û��������������ǵ�select�����Splay�����Ա�֤��̯���Ӷȡ�
    Node *select(Node *root, int i) {
        for (Node *p = root;;) {
            p->pushdown();
            if (p->ch[0]->size + 1 == i) {
                return p;
            } else if (p->ch[0]->size >= i) {
                p = p->ch[0];
            } else {
                i -= p->ch[0]->size + 1;
                p = p->ch[1];
            }
        }
    }
    
    //���ؽ��p�����е�����
    int rank(Node *p) {
        p->Splay();
        return p->ch[0]->size + 1;
    }
    
    // ���� >= a �Ľ�㣬��û���򷵻�null, ��֮��û��������������ý���Splay�����Ա�֤��̯���Ӷȡ�
    // ����nullʱ����������֤���Ӷȣ� select(root, root->size)->Splay();
    template <class T> Node *lower_bound(Node *root, T a) {
        Node *ret = null;
        for (Node *p = root; p != null; ) {
            p->pushdown();
            if (a < p->key) {
                p = p->ch[1];
            } else {
                ret = p;
                p = p->ch[0];
            }
        }
        return ret;
    }
    
    //��������������֮�ϲ�(���Դ���null)��p�н�����q�н�����ߡ�
    Node *merge(Node *p, Node *q) {
        p->f = q->f = null;
        if (p == null) return q;
        if (q == null) return p;
        q = select(q, 1);
        q->Splay();
        q->set(0, p);
        q->update();
        return q;
    }
    
    //��pΪ������qΪp���Ҷ���ʱ����ת��p��q�����н�㣬��������
    Node *reverse(Node *p, Node *q) {
        swap(p->ch[0], q->ch[0]);
        p->ch[1] = q->ch[1];
        q->ch[1] = p;
        q->f = null;
        p->fix();
        q->fix();
        p->update();
        q->update();
        p->ch[0]->rvs ^= 1;
        return q;
    }
    
    //��ת��l��Ԫ�ص���r��Ԫ�أ������������±��1��ʼ��
    Node *reverse(Node *root, int l, int r) {
        if (l >= r) return root;
        Node *p = select(root, l);
        p->Splay();
        Node *q = select(p, r);
        q->Splay(p);
        return reverse(p, q);
    }
    
    //��p��ǰһλ����q����������
    Node *insert(Node *p, Node *q) {
        p->Splay();
        if (p->ch[0] == null) {
            p->set(0, q);
        } else {
            Node *t = select(p, p->ch[0]->size);
            t->Splay(p);
            t->set(1, q);
            t->update();
        }
        p->update();
        return p;
    }
    
    //�ڵ�i��Ԫ��ǰ������q����������
    Node *insert(Node *root, Node *q, int i) {
        if (i > root->size) {
            Node *p = select(root, root->size);
            p->Splay();
            p->set(1, q);
            p->update();
            return p;
        } else {
            Node *p = select(root, i);
            return insert(p, q);
        }
    }
    
    //ɾ����pΪ��������
    Node *erase(Node *p) {
        if (p->f != null) {
            Node *q = p->f;
            q->pushdown();
            q->set(q->ch[1] == p, null);
            q->update();
            q->Splay();
            return q;
        } else {
            return null;
        }
    }
    
    //ɾ����l������r����㣬��������
    Node *erase(Node *root, int l, int r) {
        if (l > r) return root;
        if (l == r) {
            Node *p = select(root, l);
            p->Splay();
            return merge(p->ch[0], p->ch[1]);
        } else {
            Node *p = select(root, l);
            p->Splay();
            Node *q = select(p, r);
            q->Splay(p);
            return merge(p->ch[0], q->ch[1]);
        }
    }
    
    //�п����p���������֮��ıߣ������������ĸ��Լ�ԭ���ĸ���
    pair <Node *, Node *> split(Node *p) {
        Node *q = p->ch[0];
        p->ch[0] = null;
        q->f = null;
        p->update();
        p->Splay();
        return make_pair(q, p);
    }
}
