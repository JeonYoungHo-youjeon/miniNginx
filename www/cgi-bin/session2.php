<?php
    session_start();
?>
<!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>세션</title>
</head>
<body>
    <h2>세션</h2>
    <p>아이디 : <?=$_SESSION['userid']?></p>
    <p>이름 : <?=$_SESSION['username']?></p>
    <p>모든 세션 : <?=print_r($_SESSION)?></p>
</body>
</html>