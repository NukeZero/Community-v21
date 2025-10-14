
#ifndef	__FLCMDOPTION_H__
#define	__FLCMDOPTION_H__


#include "FLUICommon.h"

#include <vector>


class	FLCmdOption
{
public:

	FLCmdOption();
	~FLCmdOption();

	void	AddOption( const tstring& strDefinedOption );
	void	AddOptions( const std::vector< tstring >& vecDefinedOptions );

	bool	AddValue( const tstring& strName, const tstring& strValue );

	bool	IsSupplied( const tstring& strName )	const;
	tstring	GetFirstValue( const tstring& strName )	const;
	bool	GetValues( const tstring& strName, std::vector< tstring >& vecValues )	const;

private:

	struct T_OPTION
	{
		tstring					strName;
		std::vector< tstring >	vecValues;
	};

	bool			IsDefinedOption( const tstring& strName )	const;
	const T_OPTION*	Find( const tstring& strName )	const;
	T_OPTION*		Find( const tstring& strName );

private:

	typedef	std::vector< tstring >	DEFINED_OPTION_LIST;
	typedef	std::vector< T_OPTION >	OPTION_LIST;

	DEFINED_OPTION_LIST	m_vecDefinedOptions;
	OPTION_LIST			m_vecOptions;

};



#endif