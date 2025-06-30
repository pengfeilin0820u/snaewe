////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: October 12, 2002

#ifndef FACTORY_INC_
#define FACTORY_INC_

#include "TypeInfo.h"
#include "AssocVector.h"
#include <exception>

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class DefaultFactoryError
// Manages the "Unknown Type" error in an object factory
////////////////////////////////////////////////////////////////////////////////

    struct DefaultFactoryError
    {

        struct Exception : public std::exception
        {
            const char* what() const throw() { return "Unknown Type"; }
        };

        template <typename IdentifierType, class AbstractProduct>
        static AbstractProduct* OnUnknownType(IdentifierType,AbstractProduct *)
        {
            throw Exception();
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template Factory
// Implements a generic object factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct, 
        typename IdentifierType,
        typename ProductCreator = AbstractProduct* (*)(),
        class FactoryErrorPolicy = DefaultFactoryError
    >
    class Factory 
        : public FactoryErrorPolicy
    {
    public:
        bool Register(const IdentifierType& id, ProductCreator creator)
        {
            return associations_.insert(
                IdToProductMap::value_type(id, creator)).second;
        }
        
        bool Unregister(const IdentifierType& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const IdentifierType& id)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
            {
                return (i->second)();
            }

            AbstractProduct *dummy;

            return OnUnknownType(id,dummy);
        }
        
    private:
        typedef AssocVector<IdentifierType, ProductCreator> IdToProductMap;
        IdToProductMap associations_;
    };

////////////////////////////////////////////////////////////////////////////////
// class template CloneFactory
// Implements a generic cloning factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct, 
        class ProductCreator = 
            AbstractProduct* (*)(const AbstractProduct*),
        typename FactoryErrorPolicy = DefaultFactoryError
    >
    class CloneFactory
        : public FactoryErrorPolicy
    {
    public:
        bool Register(const TypeInfo& ti, ProductCreator creator)
        {
            return associations_.insert(
                IdToProductMap::value_type(ti, creator)).second;
        }
        
        bool Unregister(const TypeInfo& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const AbstractProduct* model)
        {
            if (model == 0) return 0;
            
            typename IdToProductMap::iterator i = 
                associations_.find(typeid(*model));
            if (i != associations_.end())
            {
                return (i->second)(model);
            }

            AbstractProduct *dummy;

            return OnUnknownType(TypeInfo(typeid(*model)),dummy);
        }
        
    private:
        typedef AssocVector<TypeInfo, ProductCreator> IdToProductMap;
        IdToProductMap associations_;
    };
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May 08, 2002: replaced const_iterator with iterator so that self-modifying
//      ProductCreators are supported. Also, added a throw() spec to what().
//      Credit due to Jason Fischl.
// October 12, 2002: Ported to MSVC 6 by Terje Sletteb�. Interface for Factory
//      and CloneFactory changed from using template template parameter,
//      to using class with member template, for the FactoryErrorPolicy.
////////////////////////////////////////////////////////////////////////////////

#endif // FACTORY_INC_
