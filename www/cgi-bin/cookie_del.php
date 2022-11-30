<?php
    setcookie("user", "delete", time()-10, "/");
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
        echo "쿠키가 삭제 되었습니다.";
    ?>
</body>
</html>