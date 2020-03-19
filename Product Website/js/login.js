function login(){

  var userEmail = document.getElementById("email_field").value;
  var userPass = document.getElementById("password_field").value;

  firebase.auth().signInWithEmailAndPassword(userEmail, userPass)
  .then(function() {
    sessionStorage.setItem('loggedIn', true);
    window.location.replace('../index.html');
  })
  .catch(function(error) {
      var errorCode = error.code;
      var errorMessage = error.message;
      window.alert("Error : " + errorMessage);
  });

}

