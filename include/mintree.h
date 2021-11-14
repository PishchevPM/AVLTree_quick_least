#pragma once

#include <algorithm>
#include <cassert>

namespace treens
{
    /**
     * @brief AVL-Tree with O(logN) kth least element searching and index number searching
     * @tparam KeyT type of data
     */
    template <typename KeyT>
    class MinTree final
    {
    private:
        struct MtNode
        { 
            KeyT key_;
            int height_ = 1;
            struct MtNode *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;
            unsigned long long lcount_ = 0;
        };

    public:
        /**
         * @brief Iterator for tree
         */
        class Iterator final
        {
        private:
            MtNode* node_ = nullptr;
        public:
            Iterator(MtNode* node) : node_(node) {};

            /**
             * @brief get element index
             * @return index of element 
             */
            unsigned long long position () const
            {
                MtNode* tmp = node_;
                unsigned long long sum = tmp->lcount_;
                while (tmp->parent_ != nullptr)
                {
                    if (tmp->parent_->right_ == tmp)
                        sum += tmp->parent_->lcount_ + 1;
                    tmp = tmp->parent_;
                };
                return sum;
            }
            bool operator== (const Iterator& it) const
            {
                return node_ == it.node_;
            }
            bool operator!= (const Iterator& it) const
            {
                return node_ != it.node_;
            }
            Iterator& operator++()
            {
                if (node_->right_ != nullptr)
                {
                    node_ = node_->right_;
                    while (node_->left_ != nullptr)
                        node_ = node_->left_;
                }
                else
                {
                    while (node_->parent_ != nullptr && node_->parent_->right_ == node_)
                        node_ = node_->parent_;
                    node_ = node_->parent_;
                }

                return *this;
            }
            Iterator& operator--()
            {
                if (node_->left_ != nullptr)
                {
                    node_ = node_->left_;
                    while (node_->right_ != nullptr)
                        node_ = node_->right_;
                }
                else
                {
                    while (node_->parent_ != nullptr && node_->parent_->left_ == node_)
                        node_ = node_->parent_;
                    node_ = node_->parent_;
                }
                return *this;
            }
            KeyT operator* ()
            {
                return node_->key_;
            }
            KeyT* operator-> ()
            {
                return node_;
            }

            operator KeyT& ()
            {
                return node_->key_;
            }
        };

    private:
        MtNode* top_ = nullptr;
        unsigned long long size_ = 0;
        
        MinTree (MtNode* top) : top_(top) {};

        int geth (MtNode* node)
        {
            return (node == nullptr) ? 0 : node->height_;
        }

        int getdiff (MtNode* node)
        {
            if (node == nullptr)
                return 0;
            else
                return geth(node->right_) - geth(node->left_);
        }

        void counth (MtNode* node)
        {
            node->height_ = std::max (geth(node->left_), geth(node->right_));
        }

        void lcup (MtNode* node, bool positive)
        {
            while (node->parent_ != nullptr)
            {
                if (node->parent_->left_ == node)
                    node->parent_->lcount_ = (positive) ? node->parent_->lcount_ + 1 : node->parent_->lcount_ - 1 ;
                node = node->parent_;
            }
        }

        void swap_with_parent (MtNode* node)
        {
            MtNode* par = node->parent_;
            if (par == nullptr)
                return;

            if (par->left_ == node)
            {
                par->left_ = node->right_;
                if (par->left_ != nullptr)
                    par->left_->parent_ = par;

                node->parent_ = par->parent_;
                if (node->parent_->left_ == par)
                {
                    node->parent_->left_ = node;
                    node->parent_->lcount_ = par->lcount_;
                }
                else 
                    node->parent_->right_ = node;
                
                node->right_ = par;
                par->parent_ = node;
                par->lcount_ -= node->lcount_ + 1;
            }
            else
            {
                par->right_ = node->left_;
                if (par->right_ != nullptr)
                    par->right_->parent_ = par;

                node->parent_ = par->parent_;
                if (node->parent_->left_ == par)
                {
                    node->parent_->left_ = node;
                    node->parent_->lcount_ -= par->lcount_ + 1;
                }
                else 
                    node->parent_->right_ = node;
                
                node->left_ = par;
                par->parent_ = node;
                node->lcount_ += par->lcount_ + 1;
            }
            counth (par);
            counth (node);
        }

        void rebalance (MtNode* node)
        {
            int tmp_h;

            while (node->parent_ != nullptr)
            {
                if (getdiff (node->parent_) == -2)
                {
                    node = node->parent_->left_;
                    if (getdiff (node) == 1)
                    {
                        swap_with_parent (node->right_);
                        node = node->parent_;
                    }

                    swap_with_parent (node);
                    node = node->right_;
                }
                else if (getdiff (node->parent_) == 2)
                {
                    node = node->parent_->right_;
                    if (getdiff (node) == -1)
                    {
                        swap_with_parent (node->left_);
                        node = node->parent_;
                    }

                    swap_with_parent (node);
                    node = node->left_;
                }
                
                tmp_h = node->parent_->height_;
                counth (node->parent_);
                if (node->parent_->height_ == tmp_h)
                    break;

                node = node->parent_;
            }
        }
    public:
        MinTree () {};
        /**
         * @brief Copy constructor
         */
        MinTree (const MinTree& tr)
        {
            MtNode* pt1 = tr.top_;
            top_ = new MtNode(pt1);
            MtNode* pt2 = top_;
            while (pt1 != nullptr)
            {
                if (pt1->left_ != nullptr)
                {
                    pt2->left_ = new MtNode (pt1->left_);
                    pt1 = pt1->left_;
                    pt2 = pt2->left_;
                }
                else if (pt1->right_ != nullptr)
                {
                    pt2->right_ = new MtNode (pt1->right_);
                    pt1 = pt1->right_;
                    pt2 = pt2->right_;
                }
                else
                {
                    pt1 = pt1->parent_;
                    pt2 = pt2->parent_;
                }
            }
        }

        /**
         * @brief Move constructor
         */
        MinTree (MinTree&& tr)
        {
            std::swap (top_, tr.top_);
            size_ = tr.size_;
        }

        /**
         * @brief Assign operator
         */
        MinTree& operator= (const MinTree& tr)
        {
            if (&tr == this)
                return *this;

            MinTree* temp = new MinTree(top_);
            delete temp;
            temp = new MinTree (tr);
            top_ = temp->top_;
            size_ = temp->size_;
            temp->top_ = nullptr;
        }

        MinTree& operator= (MinTree&& tr)
        {
            std::swap (top_, tr.top_);
            size_ = tr.size_;
        }

        /**
         * @brief Destructor
         */
        ~MinTree ()
        {
            while (top_ != nullptr)
            {
                if (top_->left_ != nullptr)
                    top_ = top_->left_;
                else if (top_->right_ != nullptr)
                    top_ = top_->right_;
                else
                {
                    MtNode* tmp = top_->parent_;
                    delete top_;
                    if (tmp == nullptr)
                        return;
                    else if (tmp->right_ == top_)
                        tmp->right_ = nullptr;
                    else 
                        tmp->left_ = nullptr;
                    top_ = tmp;
                }
            }
        }

        /**
         * @brief iterator to element after last
         */
        Iterator end () const
        {
            return Iterator(nullptr);
        }

        unsigned long long size() const
        {
            return size_;
        }
        /**
         * @brief find iterator to element with this value
         * @param key value need to search
         * @return Iterator to elem or end (), if not founded
         */
        Iterator find (KeyT key)
        {
            MtNode* node = top_;
            while (node != nullptr)
            {
                if (node->key_ == key)
                    return Iterator (node);
                else if (node->key_ < key)
                    node = node->right_;
                else
                    node = node->left_;
            }
            return end ();
        }

        /**
         * @brief find iterator to elem by its index number
         * @param k index number
         * @return Iterator to elem or end (), if not founded
         */
        Iterator find_pos (unsigned long long k)
        {
            unsigned long long sum = 0;
            MtNode* node = top_;
            while (node != nullptr)
            {
                if (k == sum + node->lcount_)
                    return Iterator (node);
                else if (k < sum + node->lcount_)
                    node = node->left_;
                else
                {
                    sum += node->lcount_ + 1;
                    node = node->right_;
                }
            }

            return end();
        }

        Iterator operator[] (unsigned long long k)
        {
            return find_pos (k);
        }

        /**
         * @brief iterator to first element greater then key
         * @param key required key
         * @return Iterator or end (), if not founded
         */
        Iterator upper_bound (KeyT key) const
        {
            MtNode* node = top_;
            MtNode* temp = nullptr;
            while (node != nullptr)
            {
                if (key >= node->key_)
                    node = node->right_;
                else
                {
                    temp = node;
                    node = node->left_;
                }
            }
            return Iterator(temp);
        }

        /**
         * @brief iterator to first element not less then key
         * @param key required key
         * @return Iterator or end (), if not founded
         */
        Iterator lower_bound (KeyT key) const
        {
            MtNode* node = top_;
            MtNode* temp = nullptr;
            while (node != nullptr)
            {
                if (key == node->key_)
                    return Iterator (node);
                else if (key > node->key_)
                    node = node->right_;
                else
                {
                    temp = node;
                    node = node->left_;
                }
            }
            return Iterator(temp);
        }

        /**
         * @brief insert the key or do nothing if it already exists
         * @param key insert key
         */
        void insert (KeyT key)
        {
            if (top_ == nullptr)
            {
                top_ = new MtNode();
                top_->key_ = key;
                size_ = 1;
                return;
            }

            MtNode* tmp = top_;
            while (true)
            {
                MtNode* tmp2;
                if (key == tmp->key_)
                    return;
                else if (key < tmp->key_)
                    tmp2 = tmp->left_;
                else
                    tmp2 = tmp->right_;

                if (tmp2 == nullptr)
                    break;
                tmp = tmp2;
            }

            if (key == tmp->key_)
                return;
            else if (key < tmp->key_)
            {
                assert (tmp->left_ == nullptr);
                tmp->left_ = new MtNode();
                tmp->left_->key_ = key;
                tmp->left_->parent_ = tmp;
                lcup (tmp->left_, true);
                rebalance (tmp->left_);
            }
            else
            {
                assert (tmp->right_ == nullptr);
                tmp->right_ = new MtNode();
                tmp->right_->key_ = key;
                tmp->right_->parent_ = tmp;
                lcup (tmp->right_, true);
                rebalance (tmp->right_);
            }

            size_++;
        };

        /**
         * @brief erase the key or do nothing if it is not there
         * @param key insert key
         */
        void erase (KeyT key)
        {
            MtNode* ersd = top_;
            MtNode* nd;
            while (ersd != nullptr && ersd->key_ != key)
            {
                if (ersd->key_ < key)
                    ersd = ersd->right_;
                else
                    ersd = ersd->left_;
            }

            if (ersd == nullptr)
                return;
            
            if (ersd->right_ != nullptr && ersd->left_ != nullptr)
            {
                nd = ersd->left_;
                while (nd->right_ != nullptr)
                    nd = nd->right_;
                std::swap (nd->key_, ersd->key_);
                ersd = nd;
            }
            
            lcup (ersd, false);

            size_--;
            if (ersd->left_ == nullptr)
                nd = ersd->right_;
            else
                nd = ersd->left_;

            if (ersd->parent_ == nullptr)
            {
                top_ = nd;
            }
            else
            {
                if (ersd->parent_->left_ == ersd)
                    ersd->parent_->left_ = nd;
                else 
                    ersd->parent_->right_ = nd;
            }

            if (nd != nullptr)
                nd->parent_ = ersd->parent_;

            rebalance (ersd);
            delete ersd;

        }
    };
}