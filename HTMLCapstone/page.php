 <?php
session_start();

if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
  header("location: login.php");
 
 $_SESSION = array();
  
 
// Destroy the session.
session_destroy();
 
// Redirect to login page
header("location: login.php");
exit;
 
}
?>
 <!DOCTYPE html>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<title>SmartPark</title>
	<link href="style.css" type="text/css" rel="stylesheet"/>
	
</head>

<body>


<div class="topnav">
  <a class="active" href="home.html">Home</a>
  <a class="Administrator" href="admin.html">Administrator</a>
  <a href="irrigation.html">Irrigation</a>
  <a href="modernized.html">Modernized Screen</a>
  <a href="trashcan.html">Trash Can</a>
</div>

<div style="padding-left:16px">
  
</div>







<header id="header"><p></p></header>

	<div id="container">

		<main id="center" class="column">
			
			<h1>Administrator Panel</h1>
			
										
		</main>

		<nav id="left" class="column">
			
			<h3></h3>
			

		</nav>

		<div id="right" class="column">
			
		</div>

	</div>

	<div id="footer-wrapper">
		<footer id="footer"><p></p></footer>
	</div>
<p><a href="logout.php" class="btn btn-danger">Sign Out of Your Account</a></p>
</body>

</html>