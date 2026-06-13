/*
//	File:	TCLError.h
//
//	Date:	23-Apr-2003
//
//  Author : Trias
//
*/

#ifndef __TCLError_h__
#define __TCLError_h__


#define SHOULD_NOT_OCCUR_ERROR 999

/** TCLError
	Class destined to be throwed as error and containing informations
	about the error
*/
class TCLError
{

public :

	/**
		Constructor
		@param IN message : error message
	*/
	TCLError(PMString errorKey);

	/**
		Destructor
	*/
	~TCLError();

	/**
		Accessor methods
	*/
	virtual PMString getMessage();
	virtual void setMessage(PMString message);

	/**
		Display the error message in an alert box
	*/
	virtual void DisplayError();

	virtual int32 GetErrorCode();

private :

	void SetErrorCode(PMString tclError);
	
	PMString message; // Error message
	int32 errorCode;
			
};

#endif // __TCLError_h__

