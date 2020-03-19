function logout() {
    sessionStorage.removeItem('loggedIn');
}

if(sessionStorage.getItem("loggedIn")) {
    document.getElementById("dashboard").style.display = "block";
    document.getElementById("safeRoutes").style.display = "block";
    document.getElementById("login").style.display = "none";
    document.getElementById("logout").style.display = "block";
} 
else {
    document.getElementById("dashboard").style.display = "none";
    document.getElementById("safeRoutes").style.display = "none";
    document.getElementById("login").style.display = "block";
    document.getElementById("logout").style.display = "none";
}