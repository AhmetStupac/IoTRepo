// Import the functions you need from the SDKs you need
    import { initializeApp } from "https://www.gstatic.com/firebasejs/11.1.0/firebase-app.js";
    import { getAnalytics } from "https://www.gstatic.com/firebasejs/11.1.0/firebase-analytics.js";
    // TODO: Add SDKs for Firebase products that you want to use
    // https://firebase.google.com/docs/web/setup#available-libraries

    // Your web app's Firebase configuration
    // For Firebase JS SDK v7.20.0 and later, measurementId is optional
    const firebaseConfig = {
    apiKey: "AIzaSyASP2QvGXCuXY5DO8BFrHqkjQ25jW0RUfM",
    authDomain: "nodemcu-prviprojekat.firebaseapp.com",
    databaseURL: "https://nodemcu-prviprojekat-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "nodemcu-prviprojekat",
    storageBucket: "nodemcu-prviprojekat.firebasestorage.app",
    messagingSenderId: "1019370834419",
    appId: "1:1019370834419:web:c4103a2dc39eba20576a86",
    measurementId: "G-KTV0VLYPXY"
};

    // Initialize Firebase
    const app = initializeApp(firebaseConfig);
    const analytics = getAnalytics(app);

    import { getDatabase, ref, get , set, child, update, remove }
    from "https://www.gstatic.com/firebasejs/11.1.0/firebase-database.js";

    const db = getDatabase();


    function Load() {
        selectDataByIDStatus();
    }


// select data
    function selectDataByIDStatus() {


        const status = document.getElementById("status");
        get(ref(db,"/SystemState")).then((snapshot) => {
            if (snapshot.exists()) {
                status.textContent= snapshot.val(); // ovo je tacni naziv iz firebase-a
            } else {
                alert("Podatak ne postoji");
            }
        }).catch((error) => {
            alert("Greska: " + error);
        });
    }

    // insert/update data -> mijenjanje statusa sistema
    const switchCheckBox = document.getElementById("switch");

    function changeSystemState() {


        const newState = switchCheckBox.checked ? 'UKLJUCEN' : 'ISKLJUCEN';

        set(ref(db, "/SystemState"), newState)
            .then(() => {
                alert("Uspjesno promijenjen status sistema");
            })
            .catch((error) => {
                alert("Greska pri promjeni statusa sistema: " + error);
            });

    }

    // Attach the event handler to the change event of the checkbox
    switchCheckBox.addEventListener("change", changeSystemState);



    var buzzerOn;
    //is buzzer on dugmic
    var dugmic2 = document.querySelector(".dugmic2");


    const buzzerButton = document.querySelector(".dugmic2");

    function listenAndToggleBuzzer() {


        get(ref(db, "/BuzzerOnOff")).then((snapshot) => {
            if (snapshot.exists()) {
                buzzerOn = snapshot.val();
                if (buzzerOn === 0) {
                    buzzerOn = 1;
                    controlBuzzer();
                    buzzerButton.textContent = "Turn Off the buzzer";
                }
            } else {
                alert("Podatak ne postoji");
            }
        }).catch((error) => {
            alert("Greska: " + error);
        });
    }

    function controlBuzzer() {
        set(ref(db, "/BuzzerOnOff"), buzzerOn) 
        .catch((error) => {
            alert("error" + error);
        });
    }

    function buzzerOff() {
        buzzerOn = 0;
        controlBuzzer();
    }

    // ako je buzzer ukljucen, iskljuci ga
    dugmic2.addEventListener("click", () => {
        if (buzzerOn === 1) {
            buzzerOff();
            buzzerButton.textContent = "Turn On the buzzer";
         
        } else { // ako je buzzer iskljucen
            listenAndToggleBuzzer();
        }
      });




      // Timer
    const timerSelect = document.getElementById("timerSelect");

    timerSelect.addEventListener("change", () => {
        const selectedTime = timerSelect.value;
          if (selectedTime) {
                console.log(`Selected time: ${selectedTime} minutes`); // Log the selected time
                setTimer(selectedTime);
                alert(`Timer set for ${selectedTime} minutes`);	
            }
    });

    function setTimer(minutes) {
        console.log(`Setting timer for ${minutes} minutes`);
        const milliseconds = parseInt(minutes) * 60 * 1000;

        setTimeout(() => {
            changeSystemStateToOff();
            alert(`Timer of ${minutes} minutes is up!`);
        }, milliseconds);
    }
    //nakon sto timer istekne poziva se ova funk
    function changeSystemStateToOff() {
        const newState = 'ISKLJUCEN';// sistem ce biti ukljucen jos pola sata, i ugasit ce se nakon toga

        set(ref(db, "/SystemState"), newState)
            .then(() => {
                alert("System state changed to OFF");
            })
            .catch((error) => {
                alert("Error changing system state: " + error);
            });
    }






    Load();
    window.setInterval(Load, 2000);
