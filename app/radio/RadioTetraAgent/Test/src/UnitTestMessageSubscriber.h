/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/UnitTestMessageSubscriber.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to Unit Test the SpecialisedMessageSubscriber
  * class.
  */

#ifndef UNIT_TEST_MESSAGE_SUBSCRIBER_H
#define UNIT_TEST_MESSAGE_SUBSCRIBER_H


#include "core/types/src/ta_types.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"

#include <vector>

using TA_Core::SpecialisedMessageSubscriber;


template <class TCorbaDef> class UnitTestMessageSubscriber
: public SpecialisedMessageSubscriber<TCorbaDef>
{
public:

	UnitTestMessageSubscriber(): m_count(0) {}

	virtual ~UnitTestMessageSubscriber() {}

	/**
	* receiveMessage
	*
	* Receives a message from a sender
	*
	* @param  message  A pointer to a GenericMessage object.
	*/
	virtual void receiveSpecialisedMessage(const TCorbaDef& message)
	{
		m_count ++;
		m_messages.push_back(message);
	}

	
	/**
	* getReceivedMessages
	*
	* Gets the messages that have been received by this subscriber.
	*
	* @return  the messages that have been received as
	*          a vector of GenericMessage objects.
	*/
	std::vector<TCorbaDef> getReceivedMessages() const
	{
		std::vector<TCorbaDef> copyOfMessages(m_messages);
		return copyOfMessages;
	}
	

	/**
	* getNumberOfReceivedMessages
	*
	* Gets the number of messages that have been received by this subscriber.
	*
	* @return  the number of messages received
	*/
//	unsigned long getNumberOfReceivedMessages() const
	 ta_uint32  getNumberOfReceivedMessages() const
	{
		return m_count;
	}



private:

	/**
	* the list of MessageMessageCorbaDef's that have been received, in the
	* order they were received.
	*/
	std::vector<TCorbaDef> m_messages;

	/**
	* count the number of messages received
	*/
//	unsigned long m_count;
	 ta_uint32  m_count;



}; // class UnitTestMessageSubscriber

#endif // UNIT_TEST_MESSAGE_SUBSCRIBER_H
