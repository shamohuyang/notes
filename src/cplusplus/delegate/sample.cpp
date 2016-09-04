#include "delegate.h"
#include <iostream>

using namespace std;

// ��ͨ����1
void func(int a, int b)
{
    cout << "func(" << a << ", " << b << ")" << endl;
}

// ��ͨ����2
void func2(int a, int b)
{
    cout << "func2(" << a << ", " << b << ")" << endl;
}

// ��ͨ��
class NormalClass
{
public:
    // �����ͨ��Ա����
    void normalFunc(int a, int b)
    {
        cout << "NormalClass::normalFunc(" << a << ", " << b << ")" << endl;
    }
};

// ʵ����IDelegateUnlink����
class BaseUnlinkClass : public delegates::IDelegateUnlink
{
public:
    // ����麯��
    virtual void virFunc(int a, int b)
    {
        cout << "BaseUnlinkClass::virFunc(" << a << ", " << b << ")" << endl;
    }

    // �����ͨ��Ա����
    void normalFunc(int a, int b)
    {
        cout << "BaseUnlinkClass::normalFunc(" << a << ", " << b << ")" << endl;
    }
};

class DerivedClass : public BaseUnlinkClass
{
public:
    // ����麯��
    virtual void virFunc(int a, int b)
    {
        cout << "DerivedClass::virFunc(" << a << ", " << b << ")" << endl;
    }

    // ��ľ�̬��Ա����
    static void staticFunc(int a, int b)
    {
        cout << "DerivedClass::staticFunc(" << a << ", " << b << ")" << endl;
    }
};

// ģ�庯��
template<class T>
void TFunc(T a, T b)
{
    cout << "TFunc(" << a << ", " << b << ")" << endl;
}

int main()
{
    BaseUnlinkClass *baseUnlinkClass = new BaseUnlinkClass;
    DerivedClass *derivedClass = new DerivedClass;
    NormalClass *normalClass = new NormalClass;
    
    // ����ί��
    typedef delegates::CMultiDelegate2<int, int> EvenetHandler;
    EvenetHandler event;

    // �����ͨ����
    event += newDelegate(func);
    event += newDelegate(func2);

    // ��������ͨ��Ա����
    event += newDelegate(normalClass, &NormalClass::normalFunc);
    event += newDelegate(baseUnlinkClass, &BaseUnlinkClass::normalFunc);

    // �������麯��
    event += newDelegate(baseUnlinkClass, &BaseUnlinkClass::virFunc);
    event += newDelegate(derivedClass, &DerivedClass::virFunc);
    // ע���ڶ�̬�£�ʹ�û���ָ��ʱ������ָ��Ҫ�û���ĺ���ָ�룬�������������
    // �����ڵ���ʱ����Ӧ��̬��Ҳ���ǻ������������麯��
    event += newDelegate((BaseUnlinkClass*)derivedClass, &BaseUnlinkClass::virFunc);

    // �����ľ�̬��Ա����
    event += newDelegate(&DerivedClass::staticFunc);

    // ���ģ�庯��
    event += newDelegate(TFunc<int>);
    
    // �����¼�
    event(1, 2);
    cout << endl;

    // ȥ������
    event -= newDelegate(func);

    // ȥ��baseUnlinkClass���еĺ���
    event.clear(baseUnlinkClass);

    // ȥ��derivedClass���еĺ���
    // ע�⾲̬��Ա����staticFunc����ȥ��
    event.clear(derivedClass);

    //event.clear(normalClass);
    // ������ã�normalClass����IDelegateUnlink��������
    // ����ʹ��clearȥ������ĺ���
    // Ӧ��ʹ�����·���
    event -= newDelegate(normalClass, &NormalClass::normalFunc);
    
    // �����¼�
    event(2, 3);
    cout << endl;

    return 0;
}
