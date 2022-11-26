<?php
    $cookieName = "apple";
    $cookieValue = "김사과";
    setcookie($cookieName, $cookieValue, time()+60*60, "/"); // "apple"이라는 이름으로 "김사과"라는 값을 가지고 60초간 쿠키가 지속됩니다.
?>
<!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>쿠키</title>
</head>
<body>
    <h2>쿠키</h2>
<?php
    if(isset($_COOKIE[$cookieName])){
        echo "{$cookieName}라는 이름의 쿠키가 만들어 졌습니다. 값은 {$cookieValue}입니다.";
    }
?>
</body>
</html>