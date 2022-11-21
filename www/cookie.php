<!doctype html>
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
    if (getenv('COOKIE'))
    {
        echo getenv('COOKIE');
    }
    print($_POST['검색내용']);  
?>
</body>
</html>