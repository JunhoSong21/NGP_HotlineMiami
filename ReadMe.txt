fetch 한 이후 솔루션에서 Debug, Release 한 번씩 실행 후
폴더 내에 있는 Resource 폴더 복사하여 x64 폴더 내에 exe파일이 있는 곳에 붙여넣기(or 덮어쓰기)

디버그용 함수 DEBUG_MSG를 이용하면 실행시 Visual Studio 출력창에 오류 메시지를 띄울 수 있다.
if(~~~)
	DEBUG_MSG(L"~~~가 오류 발생했습니다 %d", value)
C언어 printf처럼 %d 등의 인수를 이용해 값도 출력 가능하고 없어도 상관없다.

모르는 점은 물어보면 최대한 빠르게 답변