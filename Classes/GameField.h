//
//  GameField.h
//  kosynka
//
//  Created by dima on 29.04.15.
//
//

#ifndef __kosynka__GameField__
#define __kosynka__GameField__

//#include <stdio.h>
#include <vector>

enum class GameCart{
    NONE,
    GCART_2,
    GCART_3,
    GCART_4,
    GCART_5,
    GCART_6,
    GCART_7,
    GCART_8,
    GCART_9,
    GCART_10,
    GCART_VALET,
    GCART_DAMA,
    GCART_KOROL,
    GCART_TUZ
};

enum class MAST{NONE, CHERVY, BUBNY, PIKI, KRESTY};

enum class CartState{CS_CLOSE, CS_OPEN};

class Cart
{
    MAST _mast = MAST::NONE;
    GameCart _cart  = GameCart::NONE;
    CartState _state = CartState::CS_CLOSE;
    int _count = 0;
    void* _pObject = nullptr;
public:
    Cart(){}
    Cart(GameCart cart, MAST mast): _cart(cart), _mast(mast){}
    bool operator == (const Cart& cart) const
    {
        return _cart == cart._cart && _mast == cart._mast;
    }
    bool operator == (const void* pObj) const
    {
        return _pObject == pObj;
    }
    bool isRed() const
    {
        return _mast == MAST::CHERVY || _mast == MAST::BUBNY;
    }
    bool isBlack() const
    {
        return !isRed();
    }
    bool isEmpty() const
    {
        return _mast == MAST::NONE || _cart == GameCart::NONE;
    }
    void setObject(void* pObj) { _pObject = pObj;}
    void* getObject(){ return _pObject;}
    const void* getObject() const{ return _pObject;}
    MAST getMast() const {return _mast;}
    GameCart getCart() const {return _cart;}
};

class Coloda
{
public:
    Coloda(bool full = false);
    void fullColoda(bool reshuffle);
    void reshuffle();
    void clear();
    const Cart& getNext();
    Cart popCart();
    
    bool add(const Cart& );
    void add(const Coloda&);
    bool remove(const Cart& );
    Coloda extract(size_t idx, size_t count = -1);
    
    const std::vector<Cart>& getItems() const;
    std::vector<Cart>& getItems();
    const Cart* findByObject(const void* pObj) const;
    
    
private:
    std::vector<Cart> _carts;
    size_t _cartIndex = 0;
    
};

class GameFieldDelegate
{
public:
    virtual void* onCreateCart(const Cart&) = 0;
    virtual void onDeleteCart(const Cart&) = 0;
};

class GameField
{
public:
    GameField();
    void setDelegate(GameFieldDelegate* delegate);
    void reInit();
    bool isWin() const;
    bool moveSourceCartToDest(const void* pObj, MAST destMast);
    bool moveSourceCartToDest(const Cart&, Coloda& source, MAST destMast);
    bool moveMainCardToSource(const void* pObj, Coloda& source);
    bool moveMainCardToSource(const Cart&, Coloda& );
    
private:
    GameFieldDelegate* _delegate = nullptr;
    Coloda  _coloda;
    Coloda  _source[7];
    std::map<MAST, Coloda>  _dest={{MAST::BUBNY, Coloda()}, {MAST::CHERVY, Coloda()}, {MAST::PIKI, Coloda()}, {MAST::KRESTY, Coloda()}};
    std::vector<int> _sourceCount;
    
};

#endif /* defined(__kosynka__GameField__) */
