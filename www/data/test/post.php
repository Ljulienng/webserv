<?php

    if (isset($_POST["Message"]))
    {
        $message = $_POST["Message"];
        print "<p>Merci pour ce beau message, je cite : \" $message\" </p>";
    }
    else
    {
        print "<p>Je crois que vous avez oublié de me laisser un message !</p>";
    }
    
?>