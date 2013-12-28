/**
 * @file Augmented Tree Map - Treap implementation
 * @author  Calvin Deng (dnkywin) <calvindeng@college.harvard.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef DNKWYIN_AUGMENTED_TREEMAP
#define DNKYWIN_AUGMENTED_TREEMAP

#include <functional>
#include <ctime>
#include <cstdlib>
#include <utility>

/**
 *  @brief A container made up of (key,value) pairs, which can be retrieved
 *  based on a key, in logarithmic time. In addition, this allows for queries
 *  of an (associative, commutative) augmentation function on the map items
 *  in logarithmic time.
 *
 *  @ingroup associative_containers
 *
 *  @tparam _KeyType   Type of key objects.
 *  @tparam _ItemType  Type of mapped objects.
 *  @tparam _Augment   Augment function object type, defaults to plus<_ItemType>
 *  @tparam _Compare   Comparison function object type, defaults to less<_KeyType>
 *
 */

template< typename _KeyType, typename _ItemType, typename _Augment = std::plus< _ItemType >, typename _Compare = std::less< _KeyType > >
class augmented_treemap {
private:

    typedef std::pair<_KeyType,_ItemType> _pKI;

    _Compare __comp;
    _Augment __aug;
    _ItemType __id;
    unsigned long __tsize;

    struct _Treap_Node{
        _Treap_Node *left;
        _Treap_Node *right;
        _Treap_Node *parent;
        _KeyType key;
        _pKI value;
        _ItemType augment;
        unsigned long long priority;
        _Treap_Node( const _KeyType& initKey = _KeyType( ), const _ItemType& initItem = _ItemType( ) ) : left( 0 ), right( 0 ), parent( 0 ), value(std::make_pair(initKey,initItem)){
            priority =   (((unsigned long long)(rand())))
                        ^(((unsigned long long)(rand()))<<16)
                        ^(((unsigned long long)(rand()))<<32)
                        ^(((unsigned long long)(rand()))<<48);
            augment = value.second;
        }
    } *__root;

    void update_node(_Treap_Node *node){
        node->augment = node->value.second;
        if (node->left){
            node->augment = __aug(node->left->augment, node->augment);
        }
        if (node->right){
            node->augment = __aug(node->augment, node->right->augment);
        }
    }

    void update_ancestors(_Treap_Node *node){
        while(node->parent){
            node = node->parent;
            update_node(node);
        }
    }

    void copy_attr(_Treap_Node *node1, _Treap_Node *node2){
        node2->augment = node1->augment;
    }

    _Treap_Node* merge(_Treap_Node* node1, _Treap_Node* node2){
        if (node1==0) return node2;
        if (node2==0) return node1;
        if ( node1->priority > node2->priority ){
            node1->right = merge(node1->right, node2);
            node1->right->parent = node1;
            update_node(node1);
            return node1;
        }
        else{
            node2->left = merge(node1, node2->left);
            node2->left->parent = node2;
            update_node(node2);
            return node2;
        }
    }

    void rotate_left(_Treap_Node* node){
        node->right->parent = node->parent;

        if (node->parent == 0) __root = node->right;
        else if (node->parent->left == node) node->parent->left = node->right;
        else node->parent->right = node->right;

        node->parent = node->right;
        node->right = node->right->left;
        node->parent->left = node;
        if (node->right) node->right->parent = node;

        copy_attr(node, node->parent);
        update_node(node);
    }

    void rotate_right(_Treap_Node* node){
        node->left->parent = node->parent;

        if (node->parent == 0) __root = node->left;
        else if (node->parent->left == node) node->parent->left = node->left;
        else node->parent->right = node->left;

        node->parent = node->left;
        node->left = node->left->right;
        node->parent->right = node;
        if (node->left) node->left->parent = node;

        copy_attr(node, node->parent);
        update_node(node);
    }

    _Treap_Node* find_node(_KeyType key){
        _Treap_Node* node = __root;
        while(node){
            if (__comp(key, node->value.first)) node = node->left;
            else if (__comp(node->value.first, key)) node = node->right;
            else return node;
        }
        return 0;
    }

    void erase_node(_Treap_Node* node){
        if (node==0) {return;}

        _Treap_Node* rep_node = merge(node->left, node->right);

        if (rep_node) rep_node->parent = node->parent;

        if (node->parent == 0) __root = rep_node;
        else if (node->parent->left == node) node->parent->left = rep_node;
        else node->parent->right = rep_node;

        update_ancestors(node);
        delete node;
        __tsize--;
    }

    _Treap_Node* minimum_node(_Treap_Node* node){
        if (node==0) return 0;
        while (node->left) node = node->left;
        return node;
    }

    _Treap_Node* successor(_Treap_Node* node){
        if (node->right) return minimum_node(node->right);
        while (node->parent && node->parent->right == node) node = node->parent;
        return node->parent;
    }

    _ItemType node_query_left (_Treap_Node* node, _KeyType key_low){
        if (node == 0) return __id;
        if (__comp(node->value.first, key_low)){
            return node_query_left(node->right, key_low);
        }
        else{
            _ItemType ans = __aug( node_query_left(node->left, key_low), node->value.second );
            if (node->right) ans = __aug( ans, node->right->augment );
            return ans;
        }
    }
    _ItemType node_query_right (_Treap_Node* node, _KeyType key_high){
        if (node == 0) return __id;
        if (__comp(key_high, node->value.first)){
            return node_query_right(node->left, key_high);
        }
        else{
            _ItemType ans = __aug( node->value.second, node_query_right(node->right, key_high));
            if (node->left) ans = __aug( node->left->augment, ans );
            return ans;
        }
    }

    _ItemType node_query(_Treap_Node* node, _KeyType key_low, _KeyType key_high){ // find an efficient implementation that doesn't use minkey/maxkey
        if (node == 0) return __id;
        if (__comp(key_high, node->value.first)){
            return node_query(node->left, key_low, key_high);
        }
        else if (__comp(node->value.first, key_low)){
            return node_query(node->right, key_low, key_high);
        }
        else{
            return __aug( __aug( node_query_left(node->left, key_low), node->value.second ), node_query_right(node->right, key_high));
        }
    }

public:

    /**
     *  @brief  Creates an %augmented_treemap with no elements.
     *
     *  @param  _id    Identity element of the given function _Augment. Automatically set to 0 (for the case of plus<_KeyType>)
     *
     */
    augmented_treemap(_ItemType _id = 0) : __root( 0 ), __id( _id ), __tsize( 0 ) {srand(time(NULL)); }

    void set(_KeyType key, _ItemType item){
        _Treap_Node* node = __root;
        _Treap_Node* par = 0;
        while (node){
            par = node;
            if (__comp(key, node->value.first)) node = node->left;
            else if (__comp(node->value.first, key)) node = node->right;
            else{
                node->value.second = item;
                update_node(node);
                update_ancestors(node);
                return;
            }
        }

        node = new _Treap_Node(key, item);
        __tsize++;

        node->parent = par;

        if (par==0) __root = node;
        else if (__comp(key, par->value.first)) par->left = node;
        else par->right = node;

        update_ancestors(node);

        while (node->parent && node->parent->priority < node->priority){
            if (node->parent->left == node) rotate_right(node->parent);
            else rotate_left(node->parent);
        }

        if (node->parent == 0) __root = node;
    }

    _ItemType get(_KeyType key){
        _Treap_Node* node = find_node(key);
        if (node) return node->value.second;
    }

    void erase(_KeyType key){
        erase_node(find_node(key));
    }

    _ItemType query(_KeyType key_low, _KeyType key_high){
        return node_query(__root, key_low, key_high);
    }

    unsigned long size(){
        return __tsize;
    }

    class iterator {
    public:
        void operator++(){
            if (node->right){
                node = node->right;
                while (node->left) node = node->left;
            }
            else{
                while (node->parent && node->parent->right == node) node = node->parent;
                node = node->parent;
            }
        }
        void operator--(){
            if (node->left){
                node = node->left;
                while (node->right) node = node->right;
            }
            else{
                while (node->parent && node->parent->left == node) node = node->parent;
                node = node->parent;
            }
        }
        const _pKI& operator*() { return (static_cast<_Treap_Node*>(node))->value; }
        _pKI* operator->() const{ return &(node->value);}
        bool operator==(const iterator& it) { return node == it.node; }
        bool operator!=(const iterator& it) { return !(*this == it); }
    private:
        iterator(_Treap_Node* _node) : node(_node) {}
        _Treap_Node* node;
        friend class augmented_treemap<_KeyType, _ItemType, _Augment, _Compare>;
    };

public:
    iterator begin(){
        return iterator(minimum_node(__root));
    }
    iterator end(){
        return iterator(0);
    }
    iterator find(_KeyType key){
        return iterator(find_node(key));
    }

};

#endif
