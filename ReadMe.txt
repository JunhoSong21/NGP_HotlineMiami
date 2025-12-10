변수 수정해야 하는 부분

총알, 수류탄 데미지 수정 - Player.cpp 상단 constexpr
이벤트당 움직이는 좌표값 - Player.cpp CalcPosbyFlag의 SetPos 내부 값
캐릭터 히트박스 - DataManager.cpp 상단 constexpr

접속하는 클라이언트 수 MAX_CLIENT_NUM - Timer.cpp, DataManager.cpp