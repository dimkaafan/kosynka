//
//  GameField.cpp
//  kosynka
//
//  Created by dima on 29.04.15.
//
//

#include <random>
#include "GameField.h"

static std::random_device rd;
//static std::random

static MAST s_masts[] ={MAST::BUBNY, MAST::CHERVY, MAST::KRESTY, MAST::PIKI};
static GameCart s_carts[]={GameCart::GCART_2, GameCart::GCART_3, GameCart::GCART_4, GameCart::GCART_5, GameCart::GCART_6, GameCart::GCART_7, GameCart::GCART_8, GameCart::GCART_9, GameCart::GCART_10, GameCart::GCART_VALET, GameCart::GCART_DAMA, GameCart::GCART_TUZ};

Coloda::Coloda(bool full)
{
    if (full)
    {
        fullColoda(true);
    }
}

void Coloda::fullColoda(bool reshuffleColoda)
{
    _carts.clear();
    
    for(auto& itemM : s_masts)
        for(auto& itemC : s_carts)
            _carts.emplace_back(Cart(itemC, itemM));

    _cartIndex = 0;
    if (reshuffleColoda)
        reshuffle();
}

void Coloda::reshuffle()
{
    std::random_shuffle(_carts.begin(), _carts.end());
}

void Coloda::clear()
{
    _carts.clear();
}

const Cart& Coloda::getNext()
{
    static Cart s_empty;
    if (_cartIndex + 1 >= _carts.size())
        return s_empty;
    _cartIndex++;
    return _carts[_cartIndex];
}

Cart Coloda::popCart()
{
    Cart res;
    if (!_carts.empty())
    {
        res = _carts.back();
        _carts.pop_back();
    }
    return res;
}

bool Coloda::add(const Cart& cart)
{
    auto it = std::find(_carts.begin(), _carts.end(), cart);
    if (it == _carts.end())
    {
        _carts.emplace_back(cart);
        return true;
    }
    assert(false);
    return false;
}

void Coloda::add(const Coloda& source)
{
    auto& items = source.getItems();
    for(auto& item : items)
    {
        add(item);
    }
}

bool Coloda::remove(const Cart& cart)
{
    auto it = std::find(_carts.begin(), _carts.end(), cart);
    if (it != _carts.end())
    {
        _carts.erase(it);
        return true;
    }
    assert(false);
    return false;

}

Coloda Coloda::extract(size_t idx, size_t count)
{
    Coloda res;
    if (idx < _carts.size())
    {
        auto itB = _carts.begin();
        std::advance(itB, idx);
        auto itE = itB;
        if (count == -1)
            itE = _carts.end();
        else
        {
            if (idx + count > _carts.size())
                count = _carts.size() - idx;
            std::advance(itE, count);
        }
        for(auto it = itB; it != itE; ++it)
            res.add(*it);
        _carts.erase(itB, itE);
    }
    return res;
}

const std::vector<Cart>& Coloda::getItems() const
{
    return _carts;
}

std::vector<Cart>& Coloda::getItems()
{
    return _carts;
}

const Cart* Coloda::findByObject(const void* pObj) const
{
    auto it = std::find_if(_carts.begin(), _carts.end(), [pObj](const Cart& cart){ return cart == pObj;});
    return it == _carts.end() ? nullptr : &*it;
}

GameField::GameField()
{
    _sourceCount = {1,2,3,4,5,6,7};
}

void GameField::setDelegate(GameFieldDelegate* delegate)
{
    _delegate = delegate;
}

void GameField::reInit()
{
    for(auto& item : _coloda.getItems())
        _delegate->onDeleteCart(item);
    _coloda.fullColoda(true);
    for(auto& item : _coloda.getItems())
        item.setObject(_delegate->onCreateCart(item));
    
    for(auto& item : _dest)
        item.second.clear();
    for(auto& item : _sourceCount)
    {
        auto& source = _source[item];
        source.clear();
        for(size_t i = 0; i < item; i++)
            source.add(_coloda.popCart());
    }
}

bool GameField::isWin() const
{
    for(auto& item : _source)
        if(!item.getItems().empty())
            return false;
    return true;
}

bool GameField::moveSourceCartToDest(const void* pObj, MAST destMast)
{
    for(auto& item :  _source)
    {
        const Cart* pItem = item.findByObject(pObj);
        if (pItem)
        {
            moveSourceCartToDest(*pItem, item, destMast);
            return true;
        }
    }
    return false;
}

bool GameField::moveSourceCartToDest(const Cart& cart, Coloda& source, MAST destMast)
{
    auto it = _dest.find(destMast == MAST::NONE ? cart.getMast() : destMast);
    if (it == _dest.end())
        return false;
    auto& dest = it->second;
    auto& destItems = dest.getItems();
    if (destItems.empty())
    {
        if (cart.getCart() != GameCart::GCART_TUZ)
            return false;
    }
    else
    {
        auto backCart = destItems.back().getCart();
        if (backCart == GameCart::GCART_TUZ )
        {
            if (cart.getCart() != GameCart::GCART_2)
                return false;
        }
        else
        {
            int delta = static_cast<int>(cart.getCart()) - static_cast<int>(backCart);
            if (delta != -1)
                return false;
        }
    }
    dest.add(cart);
    source.remove(cart);
    return true;
}

bool GameField::moveMainCardToSource(const void* pObj, Coloda& source)
{
    const Cart* pItem = _coloda.findByObject(pObj);
    if (pItem)
    {
        moveMainCardToSource(&pItem, source);
        return true;
    }
    return false;
}

bool GameField::moveMainCardToSource(const Cart& cart, Coloda& source)
{
    auto& items = source.getItems();
    if (items.empty())
    {
        if (cart.getCart() != GameCart::GCART_KOROL)
            return false;
    }
    else
    {
        auto& backCart = items.back();
        if ( (backCart.isBlack() && cart.isBlack()) || (backCart.isRed() && cart.isRed()))
            return false;
        int delta = static_cast<int>(backCart.getCart()) - static_cast<int>(cart.getCart());
        if (delta != 1)
            return false;
    }
    source.add(cart);
    _coloda.remove(cart);
    return true;
}