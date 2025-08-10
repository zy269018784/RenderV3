#pragma once

typedef enum RtResult
{
	RT_SUCCESS = 0,
	/*
		VBO为空
	*/
	RT_VBO_EMPTY,
	/*
		文件打开失败
	*/
	RT_FILE_OPEN_FAILED,
}RtResult;