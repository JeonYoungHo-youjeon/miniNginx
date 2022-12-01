<?php
    session_start();
    $_SESSION["userid"] = "youjeon";
    $_SESSION["username"] = "테스트";
?>
<!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>세션</title>
</head>
<body>
    <h2>세션</h2>
    <p>세션 변수가 등록 되었습니다.</p>
</body>
</html>