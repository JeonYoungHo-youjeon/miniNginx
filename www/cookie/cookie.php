<!-- <!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>로그인</title>
</head>
<body>
    <form method="post">
    <p>검색내용 : <input type="text" name="검색내용"></p>
    <p><input type="submit" value="검색"></p>
    </form>
<?php
    // if (getenv('COOKIE'))
    // {
    //     echo getenv('COOKIE');
    // }
    // print($_POST['검색내용']);  
?>
</body>
</html> -->

<!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>로그인</title>
</head>
<body>
<?php
    if(!isset($_COOKIE['userid'])){
?>
    <form method="post" action="cookie_ok.php">
    <p>아이디 : <input type="text" name="user_id"></p>
    <p>비밀번호 : <input type="password" name="user_pw"></p>
    <p><input type="submit" value="로그인"></p>
    </form>
<?php
    }else{
?>
    <p> <?=$_COOKIE['userid']?> 님 환영합니다. </p>
    <p><a href="cookie_logout.php">로그아웃</a></p>
<?php
    }
?>
</body>
</html>