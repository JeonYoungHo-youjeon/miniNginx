<?php
    $user_id = $_POST['user_id'];
    $user_pw = $_POST['user_pw'];

    // 아이디 : apple , 비밀번호 : 123
    if($user_id == "apple" && $user_pw == "1234"){
        setcookie('userid', $user_id, time()+60*60*24*30,"/");
        echo "<script>alert('로그인 되었습니다.');location.href='cookie.php';</script>";
    }else{
        echo "<script>alert('아이디 또는 패스워드를 확인해 주세요.');history.back();</script>";
    }
?>
<!doctype html>
<html lang="ko">
 <head>
  <meta charset="UTF-8">
  <title>로그인</title>
</head>
<body>

</body>
</html>