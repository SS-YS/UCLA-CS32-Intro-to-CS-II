class Investment
{
public:
    Investment(string name, int price): m_name(name), m_price(price) {}
    virtual ~Investment() {}
    string name() const { return m_name; }
    virtual bool fungible() const { return false; };
    int purchasePrice() const { return m_price; }
    virtual string description() const = 0;
private:
    string m_name;
    int m_price;
};

class Painting: public Investment
{
public:
    Painting(string name, int price): Investment(name, price) {}
    virtual ~Painting() { cout << "Destroying " << name() << ", a painting" << endl;  }
    virtual string description() const { return "painting"; }
};

class Stock: public Investment
{
public:
    Stock(string name, int price, string ticker): Investment(name, price), m_ticker(ticker) {}
    virtual ~Stock() { cout << "Destroying " << name() << ", a stock holding" << endl;  }
    virtual bool fungible() const { return true; }
    virtual string description() const { return "stock trading as " + m_ticker; }
private:
    string m_ticker;
};

class House: public Investment
{
public:
    House(string name, int price): Investment(name, price) {}
    virtual ~House() { cout << "Destroying the house " << name() << endl;  }
    virtual string description() const { return "house"; }
};

