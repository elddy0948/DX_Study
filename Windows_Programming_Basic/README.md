# Windows Programming 알아야할 것들

## Windows Programming?

Windows application 즉, 다수의 window 들로 구성된 응용프로그램이다. 이 window를 만들고 관리하는 것이 주 목표.
window는 주 창과 메뉴, 스크롤바, 버튼 등등이 모두 window이다.

### Resource

Windows에서는 여러개의 application이 실행 가능한데, 이들은 모두 CPU, 메모리, 모니터 화면 같은 HW들을 **공유**해야 한다.
만약 이 application들이 Resource들에 무질서하게 접근하고 수정한다면 엄청난 혼란이 생길 수 있다.

그래서 Windows는 운영체제답게 역할을 수행하는데, 현재 인스턴스화되어 있는(메모리에 올라와 실행중인) application들을 관리하고, resource들을 그 application들에게 적절히 분배하는 역할을 한다.

즉, 모든 application은 Windows를 거쳐서 어떤 작업을 수행할 수 있다.

### Event, Message Queue, Message, Message Loop

Windows application은 **Event-driven programming model**을 따른다. 즉, 특정 event가 발생해야 어떤 작업을 수행한다. event의 예로는 키보드 입력, 마우스 클릭, window 생성, 크기 변경, 이동 등이 있다.

`event`가 발생하면, Windows는 대상이 되는 application에게 `message`를 보낸다. 그럼 그 event는 application의 `message queue`에 추가된다.
Windows application은 계속 `message loop`를 돌리면서 message queue에 새로온 message가 있는지 확인한다.

만약 message가 있으면 window와 연관된 `Window procedure`에 message를 넘겨준다. (모든 window에는 window procedure라는 것이 존재함)
이 **Window procedure**이 특정 message에 반응해서 실행하는 코드를 담는다. 만약 window가 직접 처리하지 않는 message들은 `default window procedure`로 넘겨주어서 거기서 처리되게 해야한다. -> Win32 API에서는 `DefWindowProc`
