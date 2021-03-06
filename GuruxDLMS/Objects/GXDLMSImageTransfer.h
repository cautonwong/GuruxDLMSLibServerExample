//
// --------------------------------------------------------------------------
//  Gurux Ltd
// 
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2. 
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#pragma once

#include "GXDLMSObject.h"
#include "GXDLMSImageActivateInfo.h"

enum IMAGE_TRANSFER_STATUS
{
    IMAGE_TRANSFER_STATUSNOT_INITIATED,
    IMAGE_TRANSFER_STATUS_INITIATED,
    IMAGE_TRANSFER_STATUS_VERIFICATION_INITIATED,
    IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL,
    IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED,
    IMAGE_TRANSFER_STATUS_ACTIVATION_INITIATED,
    IMAGE_TRANSFER_STATUS_ACTIVATION_SUCCESSFUL,
    IMAGE_TRANSFER_STATUS_ACTIVATION_FAILED  
};

class CGXDLMSImageTransfer : public CGXDLMSObject
{
	long m_ImageBlockSize;
    string m_ImageTransferredBlocksStatus;
    long m_ImageFirstNotTransferredBlockNumber;
    bool m_ImageTransferEnabled;
    IMAGE_TRANSFER_STATUS m_ImageTransferStatus;
    vector<CGXDLMSImageActivateInfo> m_ImageActivateInfo;
public:	
	//Constructor.
	CGXDLMSImageTransfer();

	//SN Constructor.
	CGXDLMSImageTransfer(unsigned short sn);

	//LN Constructor.
	CGXDLMSImageTransfer(basic_string<char> ln);

	/** 
     Holds the ImageBlockSize, expressed in octets, 
     * which can be handled by the server
    */
    long GetImageBlockSize();
    void SetImageBlockSize(long value);

    /** 
     * Provides information about the transfer status of each
     * ImageBlock. Each bit in the bit-string provides information about
     * one individual ImageBlock.
    */
    string& GetImageTransferredBlocksStatus();
    void SetImageTransferredBlocksStatus(string value);

    /** 
     Provides the ImageBlockNumber of the first ImageBlock not transferred.
     * NOTE If the Image is complete, the value returned should be above the
     * number of blocks calculated from the Image size and the ImageBlockSize
    */
    long GetImageFirstNotTransferredBlockNumber();
    void SetImageFirstNotTransferredBlockNumber(long value);

    /**      
     * Controls enabling the Image transfer process. The method can
     * be invoked successfully only if the value of this attribute is true.
     */
    bool GetImageTransferEnabled();
    void SetImageTransferEnabled(bool value);

    /**
     * Holds the status of the Image transfer process.     
     */
    IMAGE_TRANSFER_STATUS GetImageTransferStatus();
    void SetImageTransferStatus(IMAGE_TRANSFER_STATUS value);

    vector<CGXDLMSImageActivateInfo>& GetImageActivateInfo();

    // Returns amount of attributes.
	int GetAttributeCount();

    // Returns amount of methods.
	int GetMethodCount();
	
	//Get attribute values of object.
	void GetValues(vector<string>& values);

	void GetAttributeIndexToRead(vector<int>& attributes);	

	int GetDataType(int index, DLMS_DATA_TYPE& type);

	// Returns value of given attribute.
	int GetValue(int index, int selector, CGXDLMSVariant& parameters, CGXDLMSVariant& value);

	// Set value of given attribute.
	int SetValue(int index, CGXDLMSVariant& value);
};
