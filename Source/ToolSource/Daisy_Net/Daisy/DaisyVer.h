/////////////////////////////////////////////////////////////////////////////////////
//	Daisy Tool Update 1차 2007/05/22
//
// 1. ToolBar에서 선택한 항목이 - 연속적인 생성이 가능하도록 함 (ESC키 누를경우 취소)
// 2. Copy & Paste 추가 (Ctrl+c, Ctrl+v)
// 3. Ctrl 누른상태에서 연속적으로 Window 선택 가능 (Align시 사용)
// 4. 하단 ToolBar중 Align기능 부분 활성화
//
// Define - __DAISY_UPDATE_1
//
//  Daisy Tool Update 2차
//
// 1. Static의 Text Right Align기능 추가
//
//  Daisy Tool Update 3차
//
// 1. Undo, Redo 기능 추가 Ctrl에 한해서만 동작하며 Dialog 이동 시 히스토리는 파괴
// 2. Command 입력 과정에 오류가 발생할 경우가 있음
/////////////////////////////////////////////////////////////////////////////////////

//Update 기능을 사용하지 않을 경우 아래 Define을 제거 하세요.

#define __DAISY_UPDATE_1
//#define __DAISY_UPDATE_2	// Static에 오른쪽 정렬 기능 추가
#define __DAISY_UPDATE_3	// Undo, Redo 기능 추가
#define __DAISY_UPDATE_4	// Static에 정교한 정렬 기능 추가 (이 정의와 __DAISY_UPDATE_2는 상호배타적이어야 함)