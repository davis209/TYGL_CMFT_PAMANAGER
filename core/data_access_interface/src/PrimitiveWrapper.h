/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/PrimitiveWrapper.h $
 * @author:  Jade Lee
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * PrimitiveWrapper is a template class that wraps primitive types, adding to them a 'set' state
 * This removes to need to set the primitive to a magic value flagging an unset state (which is
 * typically the value 0 or -1).
 * If you get linker errors, look to head of PrimitiveWrapper source file and add in template declarations which
 * will force the compiler to compile relevant template specializations into the obj file.
 */

#pragma once
#include "core/exceptions/src/ValueNotSetException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    template <class Type>
    class PrimitiveWrapper
    {
    public:

        /**
         * PrimitiveWrapper
         *
         * This constructor creates a wrapper for a primitive that is initially unset
         *
         */
        PrimitiveWrapper() = default;

       /**
        * PrimitiveWrapper
        *
        * This constructor creates a wrapper for a primitive that is initially set
        * to the value passed in
        *
        * @param startingValue the value this container will be set to
        *
        */
        PrimitiveWrapper(const Type& startingValue)
        {
            setValue(startingValue);
        }

        /**
         * PrimitiveWrapper
         *
         * Copy constructor
         *
         * @param thePrimitiveWrapper copy source
         *
         */
        PrimitiveWrapper(const PrimitiveWrapper<Type>& thePrimitiveWrapper)
        {
            *this = thePrimitiveWrapper;
        }

        /**
         * =
         *
         * Assignment operator
         *
         * @param thePrimitiveWrapper copy source
         *
         */
        PrimitiveWrapper<Type>& operator=(const PrimitiveWrapper<Type>& thePrimitiveWrapper)
        {
            m_value = thePrimitiveWrapper.m_value;
            m_valueSet = thePrimitiveWrapper.m_valueSet;
            return *this;
        }

        virtual ~PrimitiveWrapper() = default;

        /**
         * setValue
         *
         * Updates the value of this container
         *
         * @param value the value this container will be set to
         *
         */
        void setValue(const Type& value)
        {
            m_value = value;
            m_valueSet = true;
        }

        /**
         * hasBeenSet
         *
         * Returns the 'set' status of this container
         *
         * @return true if internal value set, false otherwise
         *
         */
        bool hasBeenSet() const
        {
            return m_valueSet;
        }

        /**
         * resetValue
         *
         * Resets the 'set' state of this container, subsequent calls to getValue
         * will throw an exception if the value is not set
         */
        void resetValue()
        {
            m_valueSet = false;
        }

        /**
         * getValue
         *
         * Retrieves the value set for this container, provided the value has been set
         *
         * @return the value currently set for this container
         *
         * @throws ValueNotSetException
         *      - If the value has not been internally set, either through constructor or setValue
         *      - If resetValue was called, and value not set again with setValue
         */
        const Type& getValue() const
        {
            if (!m_valueSet)
            {
                TA_THROW(TA_Base_Core::ValueNotSetException());
            }

            return m_value;
        }

        /**
         * hasBeenSetToValue
         *
         * @return false if no value set for this wrapper, or the value set doesn't
         *              correspond to the input test value
         *
         * @param value the value to compare with
         *
         */
        bool hasBeenSetToValue(const Type& value) const
        {
            return hasBeenSet() && value == m_value;
        }

    private:

        // Internal value contained by this wrapper
        Type m_value = Type();

        // the 'set' status of m_value, false if currently not set
        bool m_valueSet = false;
    };
}
