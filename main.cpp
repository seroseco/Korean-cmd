#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <fcntl.h>
#include <iomanip>
#include <io.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace {

DWORD RunCmd(const wstring& cmd_name, const vector<wstring>& args);

const map<wstring, wstring> kCommandMap = {
    {L"창문시엠디", L"__win_cmd__"},
    {L"시엠디", L"__win_cmd__"},
    {L"파워쉘", L"powershell"},
    {L"파워셸", L"powershell"},
    {L"파셸", L"powershell"},
    {L"powershell", L"powershell"},
    {L"pwsh", L"pwsh"},
    {L"터미널", L"wt"},
    {L"빠른", L"__easter_fast__"},
    {L"도와줘", L"help"},
    {L"색상검정", L"color 07"},
    {L"색상초록", L"color 20"},
    {L"색상하얀", L"color F0"},
    {L"색상파란", L"color 1F"},
    {L"깨긋", L"cls"},
    {L"파이썬", L"python"},
    {L"파이썬3", L"python3"},
    {L"깃", L"git"},
    {L"노드", L"node"},
    {L"엔피엠", L"npm"},
    {L"엔피엑스", L"npx"},
    {L"피아이피", L"pip"},
    {L"자바", L"java"},
    {L"자바컴파일", L"javac"},
    {L"고", L"go"},
    {L"러스트", L"rustc"},
    {L"카고", L"cargo"},
    {L"도커", L"docker"},
    {L"도커컴포즈", L"docker compose"},
    {L"쿠버", L"kubectl"},
    {L"테라폼", L"terraform"},
    {L"안시블", L"ansible"},
    {L"코드", L"code"},
    {L"경로", L"cd"},
    {L"색", L"color"},
    {L"출력", L"echo"},
    {L"에코", L"echo"},
    {L"그림판", L"mspaint"},
    {L"메모장", L"notepad"},
    {L"윈도우버전", L"winver"},
    {L"창문버전", L"winver"},
    {L"계산기", L"calc"},
    {L"작업관리자", L"taskmgr"},
    {L"레지스트리", L"regedit"},
    {L"서비스", L"services.msc"},
    {L"장치관리자", L"devmgmt.msc"},
    {L"디스크관리", L"diskmgmt.msc"},
    {L"이벤트뷰어", L"eventvwr.msc"},
    {L"성능모니터", L"perfmon"},
    {L"리소스모니터", L"resmon"},
    {L"시스템설정", L"msconfig"},
    {L"시스템속성", L"sysdm.cpl"},
    {L"프로그램제거", L"appwiz.cpl"},
    {L"방화벽", L"wf.msc"},
    {L"로컬정책", L"secpol.msc"},
    {L"그룹정책", L"gpedit.msc"},
    {L"제어판", L"control"},
    {L"탐색기", L"explorer"},
    {L"원격데스크톱", L"mstsc"},
    {L"화면키보드", L"osk"},
    {L"돋보기", L"magnify"},
    {L"캡처도구", L"snippingtool"},
    {L"문자표", L"charmap"},
    {L"녹음기", L"soundrecorder"},
    {L"작업스케줄러", L"taskschd.msc"},
    {L"컴퓨터관리", L"compmgmt.msc"},
    {L"사용자계정", L"netplwiz"},
    {L"공유폴더", L"fsmgmt.msc"},
    {L"로컬사용자", L"lusrmgr.msc"},
    {L"DirectX진단", L"dxdiag"},
    {L"문제기록", L"psr"},
    {L"백업복원", L"sdclt"},
    {L"업데이트", L"control update"},
    {L"연결확장", L"assoc"},
    {L"중단검사", L"break"},
    {L"부팅편집", L"bcdedit"},
    {L"접근목록", L"cacls"},
    {L"호출", L"call"},
    {L"코드페이지", L"chcp"},
    {L"디렉터리", L"chdir"},
    {L"디스크검사", L"chkdsk"},
    {L"부팅디스크검사", L"chkntfs"},
    {L"파일비교", L"comp"},
    {L"압축", L"compact"},
    {L"파일시스템변환", L"convert"},
    {L"파티션", L"diskpart"},
    {L"명령기록", L"doskey"},
    {L"드라이버", L"driverquery"},
    {L"지역끝", L"endlocal"},
    {L"파일지우기", L"erase"},
    {L"문자찾기", L"find"},
    {L"반복", L"for"},
    {L"포맷", L"format"},
    {L"파일시스템", L"fsutil"},
    {L"파일형식", L"ftype"},
    {L"이동표", L"goto"},
    {L"정책결과", L"gpresult"},
    {L"권한", L"icacls"},
    {L"조건", L"if"},
    {L"볼륨이름", L"label"},
    {L"링크", L"mklink"},
    {L"장치설정", L"mode"},
    {L"더보기", L"more"},
    {L"열린파일", L"openfiles"},
    {L"경로설정", L"path"},
    {L"일시정지", L"pause"},
    {L"이전경로", L"popd"},
    {L"인쇄", L"print"},
    {L"프롬프트", L"prompt"},
    {L"경로저장", L"pushd"},
    {L"복구", L"recover"},
    {L"주석", L"rem"},
    {L"이름바꾸기", L"rename"},
    {L"바꾸기", L"replace"},
    {L"고급복사", L"robocopy"},
    {L"지역시작", L"setlocal"},
    {L"서비스제어", L"sc"},
    {L"작업예약", L"schtasks"},
    {L"매개이동", L"shift"},
    {L"정렬", L"sort"},
    {L"드라이브연결", L"subst"},
    {L"창제목", L"title"},
    {L"기록검증", L"verify"},
    {L"볼륨", L"vol"},
    {L"관리명령", L"wmic"},
    {L"어디", L"where"},
    {L"대기", L"timeout"},
    {L"클립", L"clip"},
    {L"네트연결", L"net use"},
    {L"사용자관리", L"net user"},
    {L"네트설정", L"netsh"},
    {L"레지명령", L"reg"},
    {L"목록", L"dir"},
    {L"종료", L"taskkill"},
    {L"작업", L"tasklist"},
    {L"실행", L"start"},
    {L"대량복사", L"xcopy"},
    {L"복사", L"copy"},
    {L"이동", L"move"},
    {L"삭제", L"del"},
    {L"이름변경", L"ren"},
    {L"만들기", L"mkdir"},
    {L"지우기", L"rmdir"},
    {L"정리", L"cls"},
    {L"내용", L"type"},
    {L"찾기", L"findstr"},
    {L"비교", L"fc"},
    {L"나무", L"tree"},
    {L"트리", L"tree"},
    {L"속성", L"attrib"},
    {L"버전", L"ver"},
    {L"날짜", L"date /t"},
    {L"시간", L"time /t"},
    {L"호스트", L"hostname"},
    {L"사용자", L"whoami"},
    {L"시스템", L"systeminfo"},
    {L"네트워크", L"ipconfig"},
    {L"확인", L"ping"},
    {L"추적", L"tracert"},
    {L"조회", L"nslookup"},
    {L"포트", L"netstat"},
    {L"주소", L"arp"},
    {L"경로표", L"route"},
    {L"전원", L"shutdown"},
    {L"환경", L"set"},
    {L"도움", L"help"},

    // 기존 이름 호환 별칭
    {L"색상", L"color"},
    {L"경로변경", L"cd"},
    {L"폴더이동", L"cd"},
    {L"디렉터리목록", L"dir"},
    {L"디렉터리이동", L"cd"},
    {L"작업종료", L"taskkill"},
    {L"작업목록", L"tasklist"},
    {L"파일복사", L"copy"},
    {L"파일이동", L"move"},
    {L"파일삭제", L"del"},
    {L"문자출력", L"echo"},
    {L"파일이름변경", L"ren"},
    {L"폴더생성", L"mkdir"},
    {L"폴더삭제", L"rmdir"},
    {L"화면정리", L"cls"},
    {L"디렉터리생성", L"mkdir"},
    {L"디렉터리삭제", L"rmdir"},
    {L"화면지우기", L"cls"},
    {L"시스템정보", L"systeminfo"},
    {L"네트워크정보", L"ipconfig"},
    {L"네트워크설정", L"ipconfig"},
    {L"연결확인", L"ping"},
    {L"경로추적", L"tracert"},
    {L"이름조회", L"nslookup"},
    {L"핑", L"ping"},
    {L"프로세스목록", L"tasklist"},
    {L"환경변수", L"set"},
    {L"도움말", L"help"},
};

const map<wstring, map<wstring, wstring>> kOptionAliasByCommand = {
    {L"taskkill",
     {
         {L"/강제", L"/F"},
         {L"/이름", L"/IM"},
         {L"/번호", L"/PID"},
         {L"/트리", L"/T"},
     }},
    {L"tasklist",
     {
         {L"/상세", L"/V"},
         {L"/서비스", L"/SVC"},
         {L"/모듈", L"/M"},
     }},
    {L"dir",
     {
         {L"/하위", L"/S"},
         {L"/간단", L"/B"},
         {L"/와이드", L"/W"},
         {L"/페이지", L"/P"},
     }},
    {L"copy",
     {
         {L"/덮어쓰기", L"/Y"},
         {L"/확인", L"/-Y"},
         {L"/검증", L"/V"},
     }},
    {L"xcopy",
     {
         {L"/하위", L"/S"},
         {L"/빈폴더포함", L"/E"},
         {L"/자동폴더", L"/I"},
         {L"/덮어쓰기", L"/Y"},
     }},
    {L"move",
     {
         {L"/덮어쓰기", L"/Y"},
         {L"/확인", L"/-Y"},
     }},
    {L"del",
     {
         {L"/하위", L"/S"},
         {L"/조용", L"/Q"},
         {L"/강제", L"/F"},
         {L"/확인", L"/P"},
     }},
    {L"ipconfig",
     {
         {L"/전체", L"/all"},
         {L"/해제", L"/release"},
         {L"/갱신", L"/renew"},
         {L"/dns표시", L"/displaydns"},
         {L"/dns삭제", L"/flushdns"},
     }},
    {L"ping",
     {
         {L"/지속", L"/t"},
         {L"/횟수", L"/n"},
         {L"/크기", L"/l"},
         {L"/대기", L"/w"},
     }},
    {L"findstr",
     {
         {L"/대소문자무시", L"/I"},
         {L"/정규식", L"/R"},
         {L"/정확히", L"/X"},
         {L"/줄번호", L"/N"},
     }},
    {L"netstat",
     {
         {L"/모두", L"-a"},
         {L"/숫자", L"-n"},
         {L"/프로세스", L"-o"},
     }},
    {L"shutdown",
     {
         {L"/종료", L"/s"},
         {L"/재시작", L"/r"},
         {L"/로그오프", L"/l"},
         {L"/취소", L"/a"},
         {L"/시간", L"/t"},
         {L"/강제", L"/f"},
     }},
};

bool IsHelpCommand(const wstring& command) {
    return command == L"도움" || command == L"도움말";
}

void PrintUnknownCommandMessage(const wstring& command) {
    wcerr << L"'" << command
          << L"'은(는) 내부 명령, 실행할 수 있는 프로그램, 또는\n";
    wcerr << L"배치 파일이 아닙니다.\n";
}

wstring Trim(const wstring& s) {
    size_t start = 0;
    while (start < s.size() && iswspace(s[start])) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && iswspace(s[end - 1])) {
        --end;
    }
    return s.substr(start, end - start);
}

bool ReadInputLine(wstring& out_line) {
    HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);
    if (h_in != INVALID_HANDLE_VALUE) {
        wchar_t buffer[2048] = {0};
        DWORD read_chars = 0;
        if (ReadConsoleW(h_in, buffer, 2047, &read_chars, nullptr)) {
            while (read_chars > 0 &&
                   (buffer[read_chars - 1] == L'\r' || buffer[read_chars - 1] == L'\n')) {
                --read_chars;
            }
            out_line.assign(buffer, buffer + read_chars);
            return true;
        }
    }

    if (!getline(wcin, out_line)) {
        return false;
    }
    return true;
}

vector<wstring> ParseCommandLine(const wstring& line) {
    int parsed_argc = 0;
    wstring full_line = L"kcp " + line;
    wchar_t** parsed_argv = CommandLineToArgvW(full_line.c_str(), &parsed_argc);
    if (parsed_argv == nullptr || parsed_argc <= 1) {
        if (parsed_argv != nullptr) {
            LocalFree(parsed_argv);
        }
        return {};
    }

    vector<wstring> out;
    out.reserve(parsed_argc - 1);
    for (int i = 1; i < parsed_argc; ++i) {
        out.emplace_back(parsed_argv[i]);
    }
    LocalFree(parsed_argv);
    return out;
}

wstring GetCurrentDirectoryText() {
    const DWORD len = GetCurrentDirectoryW(0, nullptr);
    if (len == 0) {
        return L".";
    }

    vector<wchar_t> buffer(len);
    const DWORD written = GetCurrentDirectoryW(len, buffer.data());
    if (written == 0 || written >= len) {
        return L".";
    }
    return wstring(buffer.data());
}

DWORD RunChangeDirectory(const vector<wstring>& args) {
    if (args.empty()) {
        wcout << GetCurrentDirectoryText() << L"\n";
        return 0;
    }

    // `cd "경로"` 또는 `cd ..`처럼 첫 번째 인자를 대상 경로로 사용한다.
    const wstring target = Trim(args[0]);
    if (target.empty()) {
        wcout << GetCurrentDirectoryText() << L"\n";
        return 0;
    }

    const BOOL ok = SetCurrentDirectoryW(target.c_str());
    if (!ok) {
        wcerr << L"지정된 경로를 찾을 수 없습니다: " << target << L"\n";
        return 1;
    }
    return 0;
}

DWORD RunWindowsCmdShell() {
    wcout << L"Windows CMD로 전환합니다. 돌아오려면 `exit`를 입력하세요.\n";
    const int rc = _wsystem(L"cmd.exe");
    if (rc == -1) {
        wcerr << L"Windows CMD 실행에 실패했습니다.\n";
        return 1;
    }
    return static_cast<DWORD>(rc);
}

DWORD RunFastEasterEgg() {
    wcout << L"sudo rm -rf / 를 당신의 리눅스 컴퓨터 또는 맥북에 입력하세요.\n";
    wcout << L"그러면 빨라집니다.(다른 의미로)\n";
    return 0;
}

bool TryResolveColorName(const wstring& name, wchar_t& out_code) {
    static const map<wstring, wchar_t> kColorNameMap = {
        {L"검정", L'0'},     {L"파랑", L'1'},      {L"초록", L'2'},
        {L"청록", L'3'},     {L"빨강", L'4'},      {L"자주", L'5'},
        {L"노랑", L'6'},     {L"흰색", L'7'},      {L"회색", L'8'},
        {L"연파랑", L'9'},   {L"연초록", L'A'},    {L"연청록", L'B'},
        {L"연빨강", L'C'},   {L"연자주", L'D'},    {L"연노랑", L'E'},
        {L"밝은흰색", L'F'}, {L"하양", L'F'},      {L"흰", L'7'},
    };

    const auto it = kColorNameMap.find(name);
    if (it == kColorNameMap.end()) {
        return false;
    }
    out_code = it->second;
    return true;
}

void PrintColorHelp() {
    wcout << L"사용법: 색 <글자색> [배경색]\n";
    wcout << L"예시: 색 빨강 / 색 노랑 파랑 / 색 기본\n";
    wcout << L"지원 색상: 검정, 파랑, 초록, 청록, 빨강, 자주, 노랑, 흰색,\n";
    wcout << L"          회색, 연파랑, 연초록, 연청록, 연빨강, 연자주, 연노랑, 밝은흰색\n";
}

DWORD RunColorCommand(const vector<wstring>& args) {
    if (args.empty()) {
        PrintColorHelp();
        return 0;
    }

    if (args[0] == L"기본") {
        return RunCmd(L"color", {L"07"});
    }

    wchar_t fg = 0;
    if (!TryResolveColorName(args[0], fg)) {
        wcerr << L"알 수 없는 글자색: " << args[0] << L"\n";
        PrintColorHelp();
        return 1;
    }

    wchar_t bg = L'0';  // 배경 기본값: 검정
    if (args.size() >= 2) {
        if (!TryResolveColorName(args[1], bg)) {
            wcerr << L"알 수 없는 배경색: " << args[1] << L"\n";
            PrintColorHelp();
            return 1;
        }
    }

    wstring code;
    code.push_back(bg);
    code.push_back(fg);
    return RunCmd(L"color", {code});
}

vector<wstring> TranslateOptionAliases(const wstring& real_command,
                                       const vector<wstring>& args) {
    const auto command_it = kOptionAliasByCommand.find(real_command);
    if (command_it == kOptionAliasByCommand.end()) {
        return args;
    }

    const auto& alias_map = command_it->second;
    vector<wstring> translated;
    translated.reserve(args.size());

    for (const auto& arg : args) {
        const auto exact_it = alias_map.find(arg);
        if (exact_it != alias_map.end()) {
            translated.emplace_back(exact_it->second);
            continue;
        }

        // `/옵션` 형태를 권장하지만, `옵션`만 입력해도 변환되도록 보조한다.
        const wstring slash_arg = L"/" + arg;
        const auto slash_it = alias_map.find(slash_arg);
        if (slash_it != alias_map.end()) {
            translated.emplace_back(slash_it->second);
            continue;
        }

        translated.emplace_back(arg);
    }

    return translated;
}

wstring GetWindowsVersionText() {
    wchar_t temp_path[MAX_PATH] = {0};
    if (GetTempPathW(MAX_PATH, temp_path) == 0) {
        return L"Microsoft Windows";
    }

    wchar_t temp_file[MAX_PATH] = {0};
    if (GetTempFileNameW(temp_path, L"kcp", 0, temp_file) == 0) {
        return L"Microsoft Windows";
    }

    wstring cmd = L"cmd.exe /C ver > \"";
    cmd += temp_file;
    cmd += L"\" 2>nul";

    const int rc = _wsystem(cmd.c_str());
    if (rc == -1) {
        DeleteFileW(temp_file);
        return L"Microsoft Windows";
    }

    FILE* fp = _wfopen(temp_file, L"rt");
    if (fp == nullptr) {
        DeleteFileW(temp_file);
        return L"Microsoft Windows";
    }

    wchar_t buffer[512];
    wstring line;
    if (fgetws(buffer, static_cast<int>(sizeof(buffer) / sizeof(buffer[0])), fp) !=
        nullptr) {
        line = buffer;
    }
    fclose(fp);
    DeleteFileW(temp_file);

    while (!line.empty() &&
           (line.back() == L'\r' || line.back() == L'\n' || line.back() == L' ')) {
        line.pop_back();
    }

    if (line.empty()) {
        return L"Microsoft Windows";
    }
    return line;
}

wstring QuoteArg(const wstring& arg) {
    if (arg.empty()) {
        return L"\"\"";
    }

    const bool needs_quotes =
        arg.find_first_of(L" \t\n\v\"") != wstring::npos;
    if (!needs_quotes) {
        return arg;
    }

    wstring escaped;
    escaped.reserve(arg.size() + 2);
    escaped.push_back(L'\"');
    for (wchar_t ch : arg) {
        if (ch == L'\"') {
            escaped += L"\\\"";
        } else {
            escaped.push_back(ch);
        }
    }
    escaped.push_back(L'\"');
    return escaped;
}

void PrintHelp() {
    const vector<pair<wstring, wstring>> help_lines = {
        {L"연결확장", L"파일 확장명 연결을 보여주거나 수정합니다."},
        {L"속성", L"파일 속성을 표시하거나 바꿉니다."},
        {L"중단검사", L"확장된 CTRL+C 검사를 설정하거나 지웁니다."},
        {L"부팅편집", L"부팅 데이터베이스에서 속성을 설정합니다."},
        {L"접근목록", L"파일의 ACL을 표시하거나 수정합니다."},
        {L"호출", L"한 일괄 프로그램에서 다른 일괄 프로그램을 호출합니다."},
        {L"경로", L"현재 디렉터리 이름을 보여주거나 바꿉니다."},
        {L"코드페이지", L"활성 코드 페이지 번호를 표시하거나 설정합니다."},
        {L"디렉터리", L"현재 디렉터리 이름을 보여주거나 바꿉니다."},
        {L"디스크검사", L"디스크를 검사하고 상태 보고서를 표시합니다."},
        {L"부팅디스크검사", L"부팅 중 디스크 확인 표시를 변경합니다."},
        {L"정리", L"화면을 지웁니다."},
        {L"시엠디", L"Windows 명령 인터프리터의 새 인스턴스를 시작합니다."},
        {L"색", L"콘솔의 기본색과 배경색을 설정합니다."},
        {L"파일비교", L"두 개 또는 여러 개의 파일을 비교합니다."},
        {L"압축", L"NTFS 파일 압축 표시 또는 변경을 수행합니다."},
        {L"파일시스템변환", L"FAT 볼륨을 NTFS로 변환합니다."},
        {L"복사", L"하나 이상의 파일을 다른 위치로 복사합니다."},
        {L"날짜", L"날짜를 보여주거나 설정합니다."},
        {L"삭제", L"하나 이상의 파일을 지웁니다."},
        {L"목록", L"디렉터리의 파일 및 하위 디렉터리 목록을 보여줍니다."},
        {L"파티션", L"디스크 파티션 속성을 표시하거나 구성합니다."},
        {L"명령기록", L"명령줄 편집, 다시 호출, 매크로를 지원합니다."},
        {L"드라이버", L"현재 장치 드라이버 상태와 속성을 표시합니다."},
        {L"출력", L"메시지를 표시하거나 ECHO를 켜거나 끕니다."},
        {L"지역끝", L"배치 파일에서 환경 변경의 지역화를 끝냅니다."},
        {L"파일지우기", L"하나 이상의 파일을 지웁니다."},
        {L"나가기", L"KCP 프로그램을 종료합니다."},
        {L"비교", L"두 파일 또는 파일 집합을 비교하여 차이를 표시합니다."},
        {L"문자찾기", L"파일에서 텍스트 문자열을 검색합니다."},
        {L"찾기", L"파일에서 문자열을 검색합니다."},
        {L"반복", L"파일 집합의 각 파일에 대해 명령을 실행합니다."},
        {L"포맷", L"Windows에서 사용할 디스크를 포맷합니다."},
        {L"파일시스템", L"파일 시스템 속성을 표시하거나 구성합니다."},
        {L"파일형식", L"파일 형식 연결을 표시하거나 수정합니다."},
        {L"이동표", L"일괄 프로그램에서 이름표가 붙은 줄로 이동합니다."},
        {L"정책결과", L"컴퓨터 또는 사용자에 대한 그룹 정책 정보를 표시합니다."},
        {L"도움말", L"명령에 대한 도움말 정보를 제공합니다."},
        {L"권한", L"파일/디렉터리 ACL을 표시, 수정, 백업 또는 복원합니다."},
        {L"조건", L"일괄 프로그램에서 조건 처리를 수행합니다."},
        {L"볼륨이름", L"디스크 볼륨 이름을 만들거나 바꾸거나 지웁니다."},
        {L"만들기", L"디렉터리를 만듭니다."},
        {L"디렉터리생성", L"디렉터리를 만듭니다."},
        {L"링크", L"바로 가기 링크와 하드 링크를 만듭니다."},
        {L"장치설정", L"시스템 장치를 구성합니다."},
        {L"더보기", L"출력을 한 화면씩 표시합니다."},
        {L"이동", L"파일을 한 디렉터리에서 다른 디렉터리로 이동합니다."},
        {L"열린파일", L"원격 사용자가 연 파일 정보를 표시합니다."},
        {L"경로설정", L"실행 파일의 찾기 경로를 표시하거나 설정합니다."},
        {L"일시정지", L"배치 파일 처리를 일시 중단하고 메시지를 표시합니다."},
        {L"이전경로", L"경로저장으로 저장한 이전 디렉터리를 복원합니다."},
        {L"인쇄", L"텍스트 파일을 인쇄합니다."},
        {L"프롬프트", L"명령 프롬프트 모양을 변경합니다."},
        {L"경로저장", L"현재 디렉터리를 저장한 다음 변경합니다."},
        {L"지우기", L"디렉터리를 제거합니다."},
        {L"복구", L"불량 디스크에서 읽을 수 있는 정보를 복구합니다."},
        {L"주석", L"배치 파일에 주석을 기록합니다."},
        {L"이름변경", L"파일 이름을 바꿉니다."},
        {L"이름바꾸기", L"파일 이름을 바꿉니다."},
        {L"바꾸기", L"파일을 바꿉니다."},
        {L"폴더삭제", L"디렉터리를 제거합니다."},
        {L"고급복사", L"파일과 디렉터리 트리를 복사하는 고급 유틸리티입니다."},
        {L"환경", L"환경 변수를 표시, 설정 또는 제거합니다."},
        {L"지역시작", L"배치 파일에서 환경 변경의 지역화를 시작합니다."},
        {L"서비스제어", L"서비스를 표시하거나 구성합니다."},
        {L"작업예약", L"컴퓨터에서 실행할 명령과 프로그램을 예약합니다."},
        {L"매개이동", L"배치 파일 매개 변수의 위치를 바꿉니다."},
        {L"전원", L"컴퓨터의 로컬 또는 원격 종료를 수행합니다."},
        {L"정렬", L"입력을 정렬합니다."},
        {L"실행", L"프로그램이나 명령을 실행할 별도 창을 시작합니다."},
        {L"드라이브연결", L"경로를 드라이브 문자에 연결합니다."},
        {L"시스템", L"컴퓨터별 속성과 구성을 표시합니다."},
        {L"작업", L"현재 실행 중인 모든 작업을 표시합니다."},
        {L"종료", L"실행 중인 프로세스나 응용 프로그램을 중단합니다."},
        {L"시간", L"시스템 시간을 표시하거나 설정합니다."},
        {L"창제목", L"세션의 창 제목을 설정합니다."},
        {L"나무", L"디렉터리 구조를 그래픽으로 표시합니다."},
        {L"내용", L"텍스트 파일의 내용을 표시합니다."},
        {L"버전", L"Windows 버전을 표시합니다."},
        {L"기록검증", L"파일 기록 검증 여부를 지정합니다."},
        {L"볼륨", L"디스크 볼륨 레이블과 일련 번호를 표시합니다."},
        {L"대량복사", L"파일과 디렉터리 트리를 복사합니다."},
        {L"관리명령", L"WMI 정보를 표시합니다."},
    };

    for (const auto& [cmd, desc] : help_lines) {
        wcout << left << setw(14) << cmd << L" " << desc << L"\n";
    }
    wcout << L"\n도구에 대한 자세한 내용은 온라인 도움말의 명령줄 참조를 "
             L"참조하십시오. (https://github.com/seroseco/Korean-cmd/blob/main/COMMANDS.md)\n";
}

DWORD RunCmd(const wstring& cmd_name, const vector<wstring>& args) {
    wstring command_line = L"cmd.exe /C " + cmd_name;
    for (const auto& arg : args) {
        command_line += L" ";
        command_line += QuoteArg(arg);
    }

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi{};
    vector<wchar_t> mutable_cmd(command_line.begin(), command_line.end());
    mutable_cmd.push_back(L'\0');

    const BOOL ok =
        CreateProcessW(nullptr, mutable_cmd.data(), nullptr, nullptr, TRUE, 0, nullptr,
                       nullptr, &si, &pi);
    if (!ok) {
        wcerr << L"오류: KCP 내부 명령 실행에 실패했습니다. (코드 " << GetLastError()
              << L")\n";
        return 3;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
        exit_code = 3;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return exit_code;
}

int ExecuteKoreanCommandTokens(const vector<wstring>& tokens) {
    if (tokens.empty()) {
        return 0;
    }

    const wstring korean_command = Trim(tokens[0]);
    if (IsHelpCommand(korean_command)) {
        PrintHelp();
        return 0;
    }

    const auto it = kCommandMap.find(korean_command);
    if (it == kCommandMap.end()) {
        PrintUnknownCommandMessage(korean_command);
        return 2;
    }

    vector<wstring> args;
    args.reserve(max(0, static_cast<int>(tokens.size()) - 1));
    for (size_t i = 1; i < tokens.size(); ++i) {
        args.emplace_back(tokens[i]);
    }

    if (it->second == L"cd") {
        return static_cast<int>(RunChangeDirectory(args));
    }
    if (it->second == L"__win_cmd__") {
        return static_cast<int>(RunWindowsCmdShell());
    }
    if (it->second == L"__easter_fast__") {
        return static_cast<int>(RunFastEasterEgg());
    }
    if (it->second == L"color") {
        return static_cast<int>(RunColorCommand(args));
    }

    const vector<wstring> translated_args = TranslateOptionAliases(it->second, args);
    return static_cast<int>(RunCmd(it->second, translated_args));
}

}  // namespace

//김로이는 C++을 좋아해요.

int main() {
    // 파이프/콘솔에서 한글 입출력이 깨지지 않도록 시스템 로캘을 사용한다.
    setlocale(LC_ALL, "");
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == nullptr) {
        wcerr << L"오류: 명령행 파싱에 실패했습니다. (코드 " << GetLastError()
              << L")\n";
        return 4;
    }

    auto run_interactive = [&]() -> int {
        wcout << GetWindowsVersionText() << L"\n";
        wcout << L"한국어 명령 프롬프트 (Korean Command Prompt) v2.0.0 by 김로이 "
                 L"(seroseco)\n";
        wcout << L"\n";

        bool show_newline_before_prompt = false;
        while (true) {
            if (show_newline_before_prompt) {
                wcout << L"\n";
            }

            wcout << GetCurrentDirectoryText() << L">" << flush;
            wstring line;
            if (!ReadInputLine(line)) {
                return 0;
            }
            line = Trim(line);

            if (line.empty()) {
                // 빈 입력에서는 프롬프트를 중복 출력하지 않도록 줄바꿈을 예약하지 않는다.
                show_newline_before_prompt = false;
                continue;
            }
            if (line == L"exit" || line == L"나가기" || line == L"나가") {
                return 0;
            }

            const vector<wstring> tokens = ParseCommandLine(line);
            if (tokens.empty()) {
                wcerr << L"입력을 해석하지 못했습니다. 다시 입력해 주세요.\n";
                show_newline_before_prompt = true;
                continue;
            }
            ExecuteKoreanCommandTokens(tokens);
            show_newline_before_prompt = true;
        }
    };

    if (argc < 2) {
        LocalFree(argv);
        return run_interactive();
    }

    const wstring mode = argv[1];
    if (mode == L"/c" || mode == L"/C" || mode == L"/k" || mode == L"/K") {
        vector<wstring> mode_tokens;
        mode_tokens.reserve(max(0, argc - 2));
        for (int i = 2; i < argc; ++i) {
            mode_tokens.emplace_back(argv[i]);
        }

        const bool keep_interactive = (mode == L"/k" || mode == L"/K");
        int exit_code = 0;
        if (!mode_tokens.empty()) {
            exit_code = ExecuteKoreanCommandTokens(mode_tokens);
        }

        LocalFree(argv);
        if (keep_interactive) {
            return run_interactive();
        }
        return exit_code;
    }

    vector<wstring> direct_tokens;
    direct_tokens.reserve(max(0, argc - 1));
    for (int i = 1; i < argc; ++i) {
        direct_tokens.emplace_back(argv[i]);
    }

    const int exit_code = ExecuteKoreanCommandTokens(direct_tokens);
    LocalFree(argv);
    return exit_code;
}
