#include "../lv_watch.h"

#if 1
/******************************祈祷词英文start***************************************/

//Morning Remembrance 英文
const char azkar_MR_en[Azkar_MR_Sec][356] = 
{
    AzkarImgLogo,//"Allah - there is no deity except Him, the Ever-Living, the Sustainer of [all] existence. Neither drowsiness overtakes Him nor sleep. To Him belongs whatever is in the heavens and whatever is on the earth. Who is it that can intercede with Him except by His permission? He knows what is [presently] before them and what will be after them, and they encompass not a thing of His knowledge except for what He wills. His Kursi extends over the heavens and the earth, and their preservation tires Him not. And He is the Most High, the Most Great",
    "In The Name of Allah Most Gracious Most Merciful\nSay, He is Allah, [who is] One, Allah, the Eternal Refuge, He neither begets nor is born, Nor is there to Him any equivalent",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of daybreak, From the evil of that which He created, And from the evil of darkness when it settles, And from the evil of the blowers in knots, And from the evil of an envier when he envies",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of mankind, The Sovereign of mankind, The God of mankind, From the evil of the retreating whisperer, Who whispers [evil] into the breasts of mankind, From among the jinn and mankind",
    AzkarImgLogo,//"We have reached the morning and at this very time unto Allah belongs all sovereignty, and all praise is for Allah. None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise and He is over all things omnipotent. My Lord, I ask You for the good of this day and the good of what follows it and I take refuge in You from the evil of this day and the evil of what follows it. My Lord, I take refuge in You from laziness and senility. My Lord, I take refuge in You from torment in the Fire and punishment in the grave",
    "O Allah, by your leave we have reached the morning and by Your leave we have reached the evening, by Your leave we live and die and unto You is our resurrection",
    "O Allah, You are my Lord, none has the right to be worshipped except You, You created me and I am Your servant and I abide to Your covenant and promise as best I can, I take refuge in You from the evil of which I have committed. I acknowledge Your favour upon me and I acknowledge my sin, so forgive me, for verily none can forgive sin except You",
    "O Allah, verily I have reached the morning and call on You, the bearers of Your throne, Your angles, and all of Your creation to witness that You are Allah, none has the right to be worshipped except You, alone, without partner and that Muhammad is Your Servant and Messenger",
    "O Allah, what blessing I or any of Your creation have risen upon, is from You alone, without partner, so for You is all praise and unto You all thanks",
    "O Allah, grant my body health, O Allah, grant my hearing health, O Allah, grant my sight health. None has the right to be worshipped except You. O Allah, I take refuge with You from disbelief and poverty, and I take refuge with You from the punishment of the grave. None has the right to be worshipped except You",
    "Allah is Sufficient for me, none has the right to be worshipped except Him, upon Him I rely and He is Lord of the exalted throne",
    AzkarImgLogo,//"O Allah, I ask You for pardon and well-being in this life and the next. O Allah, I ask You for pardon and well-being in my religious and worldly affairs, and my family and my wealth. O Allah, veil my weaknesses and set at ease my dismay. O Allah, preserve me from the front and from behind and on my right and on my left and from above, and I take refuge with You lest I be swallowed up by the earth",
    "O Allah, Knower of the unseen and the seen, Creator of the heavens and the Earth, Lord and Sovereign of all things, I bear witness that none has the right to be worshipped except You. I take refuge in You from the evil of my soul and from the evil and shirk of the devil, and from committing wrong against my soul or bringing such upon another Muslim",
    "In the name of Allah with whose name nothing is harmed on earth nor in the heavens and He is The All-Seeing, The All-Knowing",
    "I am pleased with Allah as a Lord, and Islam as a religion and Muhammad as a Prophet",
    "O Ever Living, O Self-Subsisting and Supporter of all, by Your mercy I seek assistance, rectify for me all of my affairs and do not leave me to myself, even for the blink of an eye",
    "We have reached the morning and at this very time all sovereignty belongs to Allah, Lord of the worlds. O Allah, I ask You for the good of this day, its triumphs and its victories, its light and its blessings and its guidance, and I take refuge in You from the evil of this day and the evil that follows it",
    "We rise upon the fitrah of Islam, and the word of pure faith, and upon the religion of our Prophet Muhammad and the religion of our forefather Ibraheem, who was a Muslim and of true faith and was not of those who associate others with Allah",
    "How perfect Allah is and I praise Him",
    "None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise, and He is over all things omnipotent",
    "None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise, and He is over all things omnipotent",
    "How perfect Allah is and I praise Him by the number of His creation and His pleasure, and by the weight of His throne, and the ink of His words",
    "O Allah, I ask You for knowledge which is beneficial and sustenance which is good, and deeds which are acceptable",
    "I seek Allaah’s forgiveness and I turn to Him in repentance",
    "O Allaah, send prayers and peace upon our Prophet Muhammad",
};

//Evening Remembrance 英文
const char azkar_ER_en[Azkar_ER_Sec][356] = 
{
    AzkarImgLogo,//"Allah - there is no deity except Him, the Ever-Living, the Sustainer of [all] existence. Neither drowsiness overtakes Him nor sleep. To Him belongs whatever is in the heavens and whatever is on the earth. Who is it that can intercede with Him except by His permission? He knows what is [presently] before them and what will be after them, and they encompass not a thing of His knowledge except for what He wills. His Kursi extends over the heavens and the earth, and their preservation tires Him not. And He is the Most High, the Most Great",
    "In The Name of Allah Most Gracious Most Merciful\nSay, He is Allah, [who is] One, Allah, the Eternal Refuge, He neither begets nor is born, Nor is there to Him any equivalent",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of daybreak, From the evil of that which He created, And from the evil of darkness when it settles, And from the evil of the blowers in knots, And from the evil of an envier when he envies",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of mankind, The Sovereign of mankind, The God of mankind, From the evil of the retreating whisperer, Who whispers [evil] into the breasts of mankind, From among the jinn and mankind",
    AzkarImgLogo,//"We have reached the evening and at this very time unto Allah belongs all sovereignty, and all praise is for Allah. None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise and He is over all things omnipotent. My Lord, I ask You for the good of this night and the good of what follows it and I take refuge in You from the evil of this night and the evil of what follows it. My Lord, I take refuge in You from laziness and senility. My Lord, I take refuge in You from torment in the Fire and punishment in the grave",
    "I take refuge in Allah’s perfect words from the evil He has created",
    "O Allah, by Your leave we have reached the evening and by Your leave we have reached the morning, by Your leave we live and die and unto You is our return",
    "O Allah, grant my body health, O Allah, grant my hearing health, O Allah, grant my sight health. None has the right to be worshipped except You. O Allah, I take refuge with You from disbelief and poverty, and I take refuge with You from the punishment of the grave. None has the right to be worshipped except You",
    "O Allah, Knower of the unseen and the seen, Creator of the heavens and the Earth, Lord and Sovereign of all things, I bear witness that none has the right to be worshipped except You. I take refuge in You from the evil of my soul and from the evil and shirk of the devil, and from committing wrong against my soul or bringing such upon another Muslim",
    "In the name of Allah with whose name nothing is harmed on earth nor in the heavens and He is The All-Seeing, The All-Knowing",
    "I am pleased with Allah as a Lord, and Islam as a religion and Muhammad as a Prophet",
    "O Ever Living, O Self-Subsisting and Supporter of all, by Your mercy I seek assistance, rectify for me all of my affairs and do not leave me to myself, even for the blink of an eye",
    "How perfect Allah is and I praise Him",
    "None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise, and He is over all things omnipotent",
};

//After Prayer Remembrance 英文
const char azkar_Apr_en[Azkar_APR_Sec][300] = 
{
    "Allah is the greatest",
    "I ask Allah to forgive me",
    "O' Allah, You are the Peace, and You are the Source of Peace, You are Blessed, Oh Possessor of Glory and Honor",
    "O' Allah Assist me in remembering you, in thanking you, and worshipping you in the best of manners",
    "Glorious is Allah",
    "Praises are due to Allah",
    "Glorious is God, Praises are due to Allah, Allah is the greatest",
    "None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise and He is over all things omnipotent",
    "Oh Allah! We ask You to be pleased with us, reward us with the Paradise and we seek Your refuge from Your anger and the punishment of the Fire",
    AzkarImgLogo,//"Allah - there is no deity except Him, the Ever-Living, the Sustainer of [all] existence. Neither drowsiness overtakes Him nor sleep. To Him belongs whatever is in the heavens and whatever is on the earth. Who is it that can intercede with Him except by His permission? He knows what is [presently] before them and what will be after them, and they encompass not a thing of His knowledge except for what He wills. His Kursi extends over the heavens and the earth, and their preservation tires Him not. And He is the Most High, the Most Great",
    "In The Name of Allah Most Gracious Most Merciful\nSay, He is Allah, [who is] One, Allah, the Eternal Refuge, He neither begets nor is born, Nor is there to Him any equivalent",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of daybreak, From the evil of that which He created, And from the evil of darkness when it settles, And from the evil of the blowers in knots, And from the evil of an envier when he envies",
    "In The Name of Allah Most Gracious Most Merciful\nSay, I seek refuge in the Lord of mankind, The Sovereign of mankind, The God of mankind, From the evil of the retreating whisperer, Who whispers [evil] into the breasts of mankind, From among the jinn and mankind",
    "O Allah, let Your Blessings come upon Muhammad and the family of Muhammad, as you have blessed Ibrahim and his family. Truly, You are Praiseworthy and Glorious.\nAllah, bless Muhammad and the family of Muhammad, as you have blessed Ibrahim and his family. Truly, You are Praiseworthy and Glorious",
};

//Hajj and Umra 英文
const char azkar_Hau_en[Azkar_HAU_Sec][296] = 
{
   "At Your service, Allah, at Your service. At Your service, You have no partner, at Your service. Truly all praise, favour and sovereignty is Yours. You have no partner",
    "Allah is the greatest",
    AzkarImgLogo,//"Allah is the greatest, Allah is the greatest, Allah is the greatest\n\nNone has the right to be worshipped except Allah, alone, without partner.To Him belongs all sovereignty and praise and He is over all things amnipotent. None has the right to be worshipped except Allah alone. He fulfilled His promise, aided His Servant and single-handedly defeated the allies",
    "Allah is the greatest",
    "‘The best of supplications is the supplication on the day of AAarafah and the best which I and the Prophets before me have said (is):\n\nNone has the right to be worshipped except Allah, alone, without partner. To Him belongs all praise and sovereignty and He is over all things omnipotent’",
};

//Fasting 英文
const char azkar_Fas_en[Azkar_Fas_Sec][200] = 
{
    "I intend to keep the fast tomorrow in the month of ramadhaan",
    "The thirst has gone and the veins are quenched, and reward is confirmed, if Allah wills",
    "‘Allah is the greatest. O Allah, let the crescent loom above us in safety, faith, peace, and Islam, and in agreement with all that You love and pleases You. Our Lord and your Lord is Allah’",
    "I am fasting, I am fasting",
};

//When waking up 英文
const char azkar_Wwu_en[Azkar_WWU_Sec][110] = 
{
    "All praise is for Allah who gave us life after having taken it from us and unto Him is the resurrection",
    AzkarImgLogo,//"None has the right to be worshipped except Allah, alone without associate, to Him belongs sovereignty and praise and He is over all things wholly capable. How perfect Allah is, and all praise is for Allah, and none has the right to be worshipped except Allah, Allah is the greatest and there is no power nor might except with Allah, The Most High, The Supreme",
    "All praise is for Allah who restored to me my health and returned my soul and has allowed me to remember Him",
};

//Garment 英文
const char azkar_Gar_en[Azkar_Gar_Sec][228] = 
{
    "All Praise is for Allah who has clothed me with this garment and provided it for me, with no power nor might from myself",
    "O Allah, for You is all praise, You have clothed me with it (i.e. the garment), I ask You for the good of it and the good for which it was made, and I seek refuge with You from the evil of it and the evil for which it was made",
    "In the name of Allah",
};

//Abulation 英文
const char azkar_Abu_en[Azkar_Abu_Sec][160] = 
{
    "In the name of Allah",
    "I bear witness that none has the right to be worshipped except Allah, alone without partner, and I bear witness that Muhammad is His slave and Messenger",
    "O Allah, make me of those who return to You often in repentance and make me of those who remain clean and pure",
};

//Home 英文
const char azkar_Home_en[Azkar_Home_Sec][160] = 
{
    "In the name of Allah we enter and in the name of Allah we leave, and upon our Lord we place our trust",
    "In the name of Allah, I place my trust in Allah, and there is no might nor power except with Allah",
    "In the name of Allah. O Allah, I take refuge with you from all evil and evil-doers",
    "I ask You (Allah) for forgiveness",
    "In the name of Allah",
    "I bear witness that none has the right to be worshipped except Allah, alone without partner, and I bear witness that Muhammad is His slave and Messenger",
    "O Allah, make me of those who return to You often in repentance and make me of those who remain clean and pure",
};

//Mosque 英文
const char azkar_Mos_en[Azkar_Mos_Sec][240] = 
{
    "O Allah, place within my heart light, and upon my tongue light, and within my ears light, and within my eyes light, and place behind me light and in front of me light and above me light and beneath me light. O Allah, bestow upon me light",
    "I take refuge with Allah, The Supreme and with His Noble Face, and His eternal authority from the accursed devil. In the name of Allah, and prayers and peace be upon the Messenger of Allah. O Allah, open the gates of Your mercy for me",
    "In the name of Allah, and prayers and peace be upon the Messenger of Allah. O Allah, I ask You from Your favour. O Allah, guard me from the accursed devil",
};

//Athan 英文
const char azkar_Ath_en[Azkar_Ath_Sec][252] = 
{
    "One repeats just as the mu-aththin (one who calls to prayer) says, except when he says:\n\nHayya AAalas-salah (or) hayya AAalal-falah\n‘come to prayer, come to success’\n\ninstead, one should say:\n\n ‘There is no might nor power except with Allah’",
    AzkarImgLogo,//"O Allah, Owner of this perfect call and Owner of this prayer to be performed, bestow upon Muhammad al-waseelah and al-fadeelah and send him upon a praised platform which You have promised him. Verily, You never fail in Your promise’\nal-waseelah: A station in paradise",
};

//Food 英文
const char azkar_Food_en[Azkar_Food_Sec][110] = 
{
    "In the name of Allah",
    "All praise is for Allah who fed me this and provided it for me without any might nor power from myself",
    "O Allah, bless for them, that which You have provided them, forgive them and have mercy upon them",
};

//Traveling 英文
const char azkar_Tra_en[Azkar_Tra_Sec][92] =
{
    AzkarImgLogo,//"Allah is the greatest, Allah is the greatest, Allah is the greatest, How perfect He is, The One Who has placed this (transport) at our service, and we ourselves would not have been capable of that, and to our Lord is our final destiny. O Allah, we ask You for birr and taqwa in this journey of ours, and we ask You for deeds which please You. O Allah, facilitate our journey and let us cover it’s distance quickly. O Allah, You are The Companion on the journey and The Successor over the family, O Allah, I take refuge with You from the difficulties of travel, from having a change of hearts and being in a bad predicament, and I take refuge in You from an ill fated outcome with wealth and family\n\n...upon returning the same supplication is recited with the following addition:\n\n‘We return, repent, worship and praise our Lord’",
    "I place you in the trust of Allah, whose trust is never misplaced",
    "I place your religion, your faithfulness and the ends of your deeds in the trust of Allah",
};

//Other Remembrance 英文
const char azkar_OR_en[Azkar_OR_Sec][248] = 
{
    AzkarImgLogo,//"‘The righteous dream is from Allah and the bad dream is from the devil, so if anyone sees something which pleases him then he should only relate it to one whom he loves...’\nSummary of what to do upon having a bad dream:\n-Spit on your left three times\nSpit: A form of spitting comprising mainly of air with little spittle\n-Seek refuge in Allah from shaytan and the evil of what you saw\n-Do not relate it to anyone\n-Turn and sleep on the opposite side to which you were sleeping on previously",
    AzkarImgLogo,//"*Al-Witr: Supplication made before or after bowing in the witr prayer\n\nO Allah, guide me along with those whom You have guided, pardon me along with those whom You have pardoned, be an ally to me along with those whom You are an ally to and bless for me that which You have bestowed. Protect me from the evil You have decreed for verily You decree and none can decree over You.For surety, he whom you show allegiance to is never abased and he whom You take as an enemy is never honoured and mighty. O our Lord, Blessed and Exalted are You",
    AzkarImgLogo,//"1- O Allah, I am Your servant, son of Your servant, son of Your maidservant, my forelock is in Your hand (i.e. You have total mastery over), Your command over me is forever executed and Your decree over me is just. I ask You by every name belonging to You which You named Yourself with, or revealed in Your Book, or You taught to any of Your creation, or You have preserved in the knowledge of the unseen with You, that You make the Quran the life of my heart and the light of my breast, and a departure for my sorrow and a release for my anxiety",
    "2- O Allah, I take refuge in You from anxiety and sorrow, weakness and laziness, miserliness and cowardice, the burden of debts and from being over powered by men",
    "O Allah, there is no ease except in that which You have made easy, and You make the difficulty, if You wish, easy",
    "1- Never mind, may it (the sickness) be a purification, if Allah wills",
    "2- I ask Allah The Supreme, Lord of the magnificent throne to cure you",
    "To Allah we belong and unto Him is our return.O Allah, recompense me for my affliction and replace it for me with something better",
    AzkarImgLogo,//"O Allah, forgive and have mercy upon him, excuse him and pardon him, and make honourable his reception. Expand his entry, and cleanse him with water, snow, and ice, and purify him of sin as a white robe is purified of filth. Exchange his home for a better home, and his family for a better family, and his spouse for a better spouse. Admit him into the Garden, protect him from the punishment of the grave and the torment of the Fire",
    "Peace be upon you all, O inhabitants of the graves, amongst the believers and the Muslims. Verily we will, Allah willing, be united with you, we ask Allah for well-being for us and you",
    "I take refuge with Allah from the accursed devil",
    "May Allah bless for you, your family and wealth",
    "None has the right to be worshipped except Allah, alone, without partner, to Him belongs all sovereignty and praise. He gives life and causes death, and He is living and does not die. In His hand is all good and He is over all things, omnipotent",
    "When he r used to receive pleasant news, he r would say:\n\nAll Praise is for Allah by whose favour good works are accomplished",
    "Upon receiving displeasing news, he r would say:\n\nAll Praise is for Allah in all circumstances",
};

//Prayer 英文
const char azkar_Pra_en[Azkar_Pra_Sec][200] = 
{
    "How perfect You are O Allah, and I praise You. Blessed be Your name, and lofty is Your position and none has the right to be worshipped except You",
    "Read Surat Al-Fatihah and a small Surah",
    "How perfect my Lord is, The Supreme",
    "May Allah answer he who praises Him",
    "How perfect my Lord is, The Most High",
    "My Lord forgive me, My Lord forgive me",
    AzkarImgLogo,//"At-tahiyyat is for Allah. All acts of worship and good deeds are for Him. Peace and the mercy and blessings of Allah be upon you O Prophet. Peace be upon us and all of Allah’s righteous servants. I bear witness that none has the right to be worshipped except Allah and I bear witness that Muhammad is His slave and Messenger",
    AzkarImgLogo,//"At-tahiyyat is for Allah. All acts of worship and good deeds are for Him. Peace and the mercy and blessings of Allah be upon you O Prophet. Peace be upon us and all of Allah’s righteous servants. I bear witness that none has the right to be worshipped except Allah and I bear witness that Muhammad is His slave and Messenger. O Allah, send prayers upon Muhammad and the followers of Muhammad, just as You sent prayers upon Ibraheem and upon the followers of Ibraheem. Verily, You are full of praise and majesty. O Allah, send blessings upon Mohammad and upon the family of Muhammad, just as You sent blessings upon Ibraheem and upon the family of Ibraheem. Verily, You are full of praise and majesty",
    "O Allah, I take refuge in You from the punishment of the grave, from the torment of the Fire, from the trials and tribulations of life and death and from the evil affliction of Al-Maseeh Ad-Dajjal",
};

/******************************祈祷词英文end***************************************/


/******************************祈祷词音译start***************************************/

//Morning Remembrance 音译
const char azkar_MR_tr[Azkar_MR_Sec][264] = 
{
    AzkarImgLogo,//"Auzubillah himinash shaitan-nir-rajeem\nAllahu la ilaha illa huwal hayyul qayyum; laa taakhuzuhoo sinatunw wa laa nawm; lahu ma fis samawati wama fil ard; man zallazee yashfa’u indahooo illaa be iznih; ya’lamu ma baina aidihim wama khalfahum; wa laa yuheetoona beshai ‘imin ‘illahee illa be maa shaa; wasi’a kursiyyuhus samaa waati wal arda wa la ya’ooduho hifzuhumaa; wa huwal aliyul ‘azeem",
    "Bismillahir Rahmanir Rahim\nQul huwa Allahu ahad, Allah hu samad, Lam yalid wa lam yulad, Wa lam yakun lahu kufuwan ahad",
    "Bismillahir Rahmanir Rahim\nQul auzu bi rabbil-falaq, Min sharri ma khalaq, Wa min sharri ghasiqin iza waqab, Wa min sharri-naffaa-saati fil ‘uqad, Wamin sharri hasidin itha hasad",
    "Bismillahir Rahmanir Rahim\nQul a’uzu bi rabbin nas, Malikin nas, Ilaahin nas, Min sharril was waasil khannaas, Al lazee yuwas wisu fee sudoorin naas, Minal jinnati wan naas",
    AzkarImgLogo,//"Asbahna wa-asbahal-mulku lillah walhamdu lillah la ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shayin qadeer, rabbi as-aluka khayra ma fee hatha-alyawm, wakhayra ma baAAdah, wa-aAAoothu bika min sharri hatha-alyawm, washarri ma baAAdah, rabbi aAAoothu bika minal-kasal, \nwasoo-il kibar, rabbi aAAoothu bika min AAathabin fin-nar, waAAathabin fil-qabr",
    "Allahumma bika asbahna wabika amsayna, wabika nahya ,wabika namootu wa-ilaykan-nushoor",
    "Allahumma anta rabbee la ilaha illa ant, khalaqtanee wa-ana AAabduk, wa-ana AAala AAahdika wawaAAdika mas-tataAAt, aAAoothu bika min sharri ma sanaAAt, aboo-o laka biniAAmatika AAalay, wa-aboo-o bithanbee, faghfir lee fa-innahu la yaghfiruth-thunooba illa ant",
    "Allahumma innee asbahtu oshhiduk, wa-oshhidu hamalata AAarshik, wamala-ikatak, wajameeAAa khalqik, annaka antal-lahu la ilaha illa ant, wahdaka la shareeka lak, wa-anna Muhammadan AAabduka warasooluk",
    "Allahumma ma asbaha bee min niAAmatin, aw bi-ahadin min khalqik, faminka wahdaka la shareeka lak, falakal-hamdu walakash-shukr",
    "Allahumma AAafinee fee badanee, allahumma AAafinee fee samAAee, allahumma AAafinee fee basaree, la ilaha illa ant. Allahumma innee aAAoothu bika minal-kufr, walfaqr, wa-aAAoothu bika min AAathabil-qabr, la ilaha illa ant",
    "Hasbiyal-lahu la ilaha illa huwa, AAalayhi tawakkalt, wahuwa rabbul-AAarshil-Aaatheem",
    AzkarImgLogo,//"Allahumma innee as-alukal-AAafwa walAAafiyah, fid-dunya wal-akhirah, allahumma innee as-alukal-AAafwa walAAafiyah fee deenee, wadunyaya wa-ahlee, wamalee, allahummas-tur AAawratee, wa-amin rawAAatee, allahummah-fathnee min bayni yaday, wamin khalfee, waAAan yameenee, waAAan shimalee, wamin fawqee, wa-aAAoothu biAAathamatika an oghtala min tahtee",
    "Allahumma AAalimal-ghaybi washshahadah, fatiras-samawati wal-ard, rabba kulli shayin wamaleekah, ashhadu an la ilaha illa ant, aAAoothu bika min sharri nafsee wamin sharrish-shaytani washirkih, waan aqtarifa AAala nafsee soo-an aw ajurrahu ila muslim",
    "Bismil-lahil-lathee la yadurru maAAas-mihi shay-on fil-ardi wala fis-sama-i wahuwas-sameeAAul-AAaleem",
    "Radeetu billahi rabban wabil-islami deenan wabiMuhammadin nabiyya",
    "Ya hayyu ya qayyoom, birahmatika astagheeth, aslih lee sha/nee kullah, wala takilnee ila nafsee tarfata AAayn",
    "Asbahna wa-asbahal-mulku lillahi rabbil-AAalameen, allahumma innee as-aluka khayra hathal-yawm, fat-hahu, wanasrahu, wanoorahu, wabarakatahu, wahudahu, wa-aAAoothu bika min sharri ma feehi, washarri ma baAAdah",
    "Asbahna AAala fitratil-islam, waAAala kalimatil-ikhlas, waAAala deeni nabiyyina Muhammad waAAala millati abeena Ibraheem, haneefan musliman wama kana minal-mushrikeen",
    "Subhanal-lahi wabihamdih",
    "La ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shay-in qadeer",
    "La ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shay-in qadeer",
    "Subhanal-lahi wabihamdih, AAadada khalqihi warida nafsih, wazinata AAarshih, wamidada kalimatih",
    "Allahumma innee as-aluka AAilman nafiAAan, warizqan tayyiban, waAAamalan mutaqabbalan",
    "Astaghfirullaaha wa 'atoobu 'ilayhi",
    "Allaahumma salli wa sallim 'alaa Nabiyyinaa Muhammadin",
};

//Evening Remembrance 音译
const char azkar_ER_tr[Azkar_ER_Sec][252] =
{
    AzkarImgLogo,//"Auzubillah himinash shaitan-nir-rajeem\nAllahu la ilaha illa huwal hayyul qayyum; laa taakhuzuhoo sinatunw wa laa nawm; lahu ma fis samawati wama fil ard; man zallazee yashfa’u indahooo illaa be iznih; ya’lamu ma baina aidihim wama khalfahum; wa laa yuheetoona beshai ‘imin ‘illahee illa be maa shaa; wasi’a kursiyyuhus samaa waati wal arda wa la ya’ooduho hifzuhumaa; wa huwal aliyul ‘azeem",
    "Bismillahir Rahmanir Rahim\nQul huwa Allahu ahad, Allah hu samad, Lam yalid wa lam yulad, Wa lam yakun lahu kufuwan ahad",
    "Bismillahir Rahmanir Rahim\nQul auzu bi rabbil-falaq, Min sharri ma khalaq, Wa min sharri ghasiqin iza waqab, Wa min sharri-naffaa-saati fil ‘uqad, Wamin sharri hasidin itha hasad",
    "Bismillahir Rahmanir Rahim\nQul a’uzu bi rabbin nas, Malikin nas, Ilaahin nas, Min sharril was waasil khannaas, Al lazee yuwas wisu fee sudoorin naas, Minal jinnati wan naas",
    AzkarImgLogo,//"Amsayna wa-amsal-mulku lillah walhamdu lillah la ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shayin qadeer, rabbi as-aluka khayra ma fee hathihil-laylah, wakhayra ma baAAdaha, wa-aAAoothu bika min sharri hathihil-laylah, washarri ma baAAdaha, rabbi aAAoothu bika minal-kasal, wasoo-il kibar, rabbi aAAoothu bika min AAathabin fin-nar, waAAathabin fil-qabr",
    "aAAoothu bikalimatil-lahit-tammati min sharri ma khalaq",
    "Allahumma bika amsayna, wabika asbahna, wabika nahya wabika namootu wa-ilaykal-maseer",
    "Allahumma AAafinee fee badanee, allahumma AAafinee fee samAAee, allahumma AAafinee fee basaree, la ilaha illa ant. Allahumma innee aAAoothu bika minal-kufr, walfaqr, wa-aAAoothu bika min AAathabil-qabr, la ilaha illa ant",
    "Allahumma AAalimal-ghaybi washshahadah, fatiras-samawati wal-ard, rabba kulli shayin wamaleekah, ashhadu an la ilaha illa ant, aAAoothu bika min sharri nafsee wamin sharrish-shaytani washirkih, waan aqtarifa AAala nafsee soo-an aw ajurrahu ila muslim",
    "Bismil-lahil-lathee la yadurru maAAas-mihi shay-on fil-ardi wala fis-sama-i wahuwas-sameeAAul-AAaleem",
    "Radeetu billahi rabban wabil-islami deenan wabiMuhammadin nabiyya",
    "Ya hayyu ya qayyoom, birahmatika astagheeth, aslih lee sha/nee kullah, wala takilnee ila nafsee tarfata Aaayn",
    "Subhanal-lahi wabihamdih",
    "La ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shay-in qadeer",
};

//After Prayer Remembrance 音译
const char azkar_Apr_tr[Azkar_APR_Sec][184] = 
{
    "Allah-hu Akbar",
    "Astagfirullah",
    "Alla'humma antas salaam wa min kas salaam taba rakta ya dhal ja laa li wal ik raam",
    "Allahumma a'inni ala dhikrika wa shukrika wa husni ibadatika",
    "Sub-han Allah",
    "Alhamdu lillah",
    "Allah hu Akbar",
    "La ilaha illa allahu wah dahu la shareeka laahu, lahul mulku wa lahul hamdu, wa huwa 'ala kullee shay'in qadeer",
    "Allahumma inni aas-aluka ridaka wal jannah, Wa aa'uzhu bika min sakhatika wan-nar",
    AzkarImgLogo,//"Auzubillah himinash shaitan-nir-rajeem\nAllahu la ilaha illa huwal hayyul qayyum; laa taakhuzuhoo sinatunw wa laa nawm; lahu ma fis samawati wama fil ard; man zallazee yashfa’u indahooo illaa be iznih; ya’lamu ma baina aidihim wama khalfahum; wa laa yuheetoona beshai ‘imin ‘illahee illa be maa shaa; wasi’a kursiyyuhus samaa waati wal arda wa la ya’ooduho hifzuhumaa; wa huwal aliyul ‘azeem",
    "Bismillahir Rahmanir Rahim\nQul huwa Allahu ahad, Allah hu samad, Lam yalid wa lam yulad, Wa lam yakun lahu kufuwan ahad",
    "Bismillahir Rahmanir Rahim\nQul auzu bi rabbil-falaq, Min sharri ma khalaq, Wa min sharri ghasiqin iza waqab, Wa min sharri-naffaa-saati fil ‘uqad, Wamin sharri hasidin itha hasad",
    "Bismillahir Rahmanir Rahim\nQul a’uzu bi rabbin nas, Malikin nas, Ilaahin nas, Min sharril was waasil khannaas, Al lazee yuwas wisu fee sudoorin naas, Minal jinnati wan naas",
    AzkarImgLogo,//"Allah humma salli 'ala muhammadin wa'ala ali muhammadin,kama sal'layta 'ala ibraheema wa'ala ali ibraheema innaka hameedum majeed. Allah humma baraka 'ala muhammadin wa'ala ali muhammadin, kama barak ta 'ala ibraheema wa 'ala ali ibraheema innaka hameedum majeed",
};

//Hajj and Umra 音译
const char azkar_Hau_tr[Azkar_HAU_Sec][236] = 
{
    "Labbayka llāhumma labbayk(a), labbayka lā sharīka laka labbayk(a), inna l-ḥamda wa n-ni’mata, laka wa l-mulk(a), lā sharīka lak",
    "Allahu akbar",
    "Allahu akbar, Allahu akbar, Allahu akbar\n\nLa ilaha illal-lahu wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shayin qadeer, la ilaha illal-lahu wahdah, anjaza waAAdah, wanasara AAabdah, wahazamal -ahzaba wahdah",
    "Allahu akbar",
    "La ilaha illal-lahu wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shayin qadeer",
};

//Fasting 音译
const char azkar_Fas_tr[Azkar_Fas_Sec][144] = 
{
    "Bisawmi ghadin nawaytu min shari ramadhaan",
    "Thahabath-thama-o wabtallatil-AAurooq, wathabatal-ajru in shaal-lah",
    "Allahu akbar, allahumma ahillahu AAalayna bil-amni wal-eeman, wassalamati wal-islam, wattawfeeiqi lima tuhibbu watarda, rabbuna warabbukal-lah",
    "Innee sa-im, innee sa-im",
};

//When waking up 音译
const char azkar_Wwu_tr[Azkar_WWU_Sec][232] = 
{
    "Alhamdu lillahil-lathee ahyana baAAda ma amatana wa-ilayhin-nushoor",
    "La ilaha illal-lahu wahdahu la shareeka lah, lahul-mulku walahul-hamd, wahuwa AAala kulli shay-in qadeer, subhanal-lah, walhamdu lillah, wala ilaha illal-lah wallahu akbar, wala hawla wala quwwata illa billahil-AAaliyyil Aaatheem",
    "Alhamdu lillahil-lathee AAafanee fee jasadee waradda AAalayya roohee wa-athina lee bithikrih",
};

//Garment 音译
const char azkar_Gar_tr[Azkar_Gar_Sec][144] = 
{
    "Alhamdu lillahil-lathee kasanee hatha (aththawb) warazaqaneehi min ghayri hawlin minnee wala quwwah",
    "Allahumma lakal-hamdu anta kasawtaneeh, as-aluka min khayrihi wakhayri ma suniAAa lah, wa-aAAoothu bika min sharrihi washarri ma suniAAa lah",
    "Bismil-lah",
};

//Abulation 音译
const char azkar_Abu_tr[Azkar_Abu_Sec][104] = 
{
    "Bismil-lah",
    "Ashhadu an la ilaha illal-lahu wahdahu la shareeka lah, wa-ashhadu anna Muhammadan AAabduhu warasooluh",
    "Allahummaj-AAalnee minat-tawwabeena wajAAalnee minal-mutatahhireen",
};

//Home 音译
const char azkar_Home_tr[Azkar_Home_Sec][104] = 
{
    "Bismil-lahi walajna, wabismil-lahi kharajna, waAAala rabbina tawakkalna",
    "Bismil-lah, tawakkaltu AAalal-lah, wala hawla wala quwwata illa billah",
    "Bismil-lah. allahumma innee aAAoothu bika minal-khubthi wal-khaba-ith",
    "Ghufranak",
    "Bismil-lah",
    "Ashhadu an la ilaha illal-lahu wahdahu la shareeka lah, wa-ashhadu anna Muhammadan AAabduhu warasooluh",
    "Allahummaj-AAalnee minat-tawwabeena wajAAalnee minal-mutatahhireen",
};

//Mosque 音译
const char azkar_Mos_tr[Azkar_Mos_Sec][228] = 
{
    "Allahumma ijAAal fee qalbee noora, wafee lisanee noora, wajAAal fee samAAee noora,wajAAal fee basaree noora, wajAAal min khalfee noora, wamin amamee noora ,wajAAal min fawqee noora, wamin tahtee noora, allahumma aAAtinee noora",
    "aAAoothu billahil-AAatheem wabiwajhihil-kareem wasultanihil-qadeem minash-shaytanir-rajeem, [bismil-lah, wassalatu] [wassalamu AAala rasoolil-lah] , allahumma iftah lee abwaba rahmatik",
    "Bismil-lah wassalatu wassalamu AAala rasoolil-lah, allahumma innee as-aluka min fadlik, allahumma iAAsimnee minash-shaytanir-rajeem",
};

//Athan 音译
const char azkar_Ath_tr[Azkar_Ath_Sec][240] =
{
    "One repeats just as the mu-aththin (one who calls to prayer) says, except when he says:\n\nHayya AAalas-salah (or) hayya AAalal-falah\n‘come to prayer, come to success’\n\ninstead, one should say:\n\n ‘La hawla wala quwwata illa billah.’",
    "Allahumma rabba hathihid-daAAwatit-tammah, wassalatil-qa-imah ati Muhammadan alwaseelata wal-fadeelah, wabAAath-hu maqaman mahmoodan allathee waAAadtah, innaka la tukhliful-meeAAad",
};

//Food 音译
const char azkar_Food_tr[Azkar_Food_Sec][96] = 
{
    "Bismil-lah",
    "Alhamdu lillahil-lathee atAAamanee hatha warazaqaneehi min ghayri hawlin minnee wala quwwah",
    "Allahumma barik lahum feema razaqtahum, waghfir lahum warhamhum",
};

//Traveling 音译
const char azkar_Tra_tr[Azkar_Tra_Sec][64] = 
{
    AzkarImgLogo,//"Allahu akbar, Allahu akbar, Allahu akbar, subhanal-lathee sakhkhara lana hatha wama kunna lahu muqrineen, wa-inna ila rabbina lamunqaliboon, allahumma inna nas-aluka fee safarina hatha albirra wattaqwa, waminal-AAamali ma tarda, allahumma hawwin AAalayna safarana hatha, watwi AAanna buAAdah, allahumma antas-sahibu fis-safar, walkhaleefatu fil-ahl, allahumma innee aAAoothu bika min waAAtha-is-safar, waka-abatil-manthar, wasoo-il-munqalabi fil-mali wal-ahl\n\n\n...upon returning the same supplication is recited with the following addition:\n\n\nAyiboona, ta-iboona, AAabidoona, lirabbina hamidoon",
    "AstawdiAAukumul-lah, allathee la tadeeAAu wada-iAAuh",
    "AstawdiAAul-laha deenak, wa-amanatak, wakhawateema AAamalik",
};

//Other Remembrance 音译
const char azkar_OR_tr[Azkar_OR_Sec][164] = 
{
    AzkarImgLogo,//"‘The righteous dream is from Allah and the bad dream is from the devil, so if anyone sees something which pleases him then he should only relate it to one whom he loves...’\nSummary of what to do upon having a bad dream:\n-Spit on your left three times\nSpit: A form of spitting comprising mainly of air with little spittle\n-Say \"\"Auzu billahi minash shaitan rajeem\"\"\n-Do not relate it to anyone\n-Turn and sleep on the opposite side to which you were sleeping on previously",
    AzkarImgLogo,//"Allahummah-dinee feeman hadayt, waAAafinee feeman AAafayt, watawallanee feeman tawallayt, wabarik lee feema aAAtayt, waqinee sharra ma qadayt, fa-innaka taqdee wala yuqda AAalayk, innahu la yathillu man walayt, [wala yaAAizzu man AAadayt], tabarakta rabbana wataAAalayt",
    AzkarImgLogo,//"1- Allahumma innee AAabduk, ibnu AAabdik, ibnu amatik, nasiyatee biyadik, madin fiyya hukmuk, AAadlun fiyya qada-ok, as-aluka bikulli ismin huwa lak, sammayta bihi nafsak, aw anzaltahu fee kitabik, aw AAallamtahu ahadan min khalqik awis-ta/tharta bihi fee AAilmil-ghaybi AAindak, an tajAAalal-Qurana rabeeAAa qalbee, wanoora sadree, wajalaa huznee wathahaba hammee",
    "2- Allahumma innee aAAoothu bika minal-hammi walhuzn, walAAajzi walkasali walbukhli waljubn, wadalAAid-dayni waghalabatir-rijal",
    "Allahumma la sahla illa ma jaAAaltahu sahla, wa-anta tajAAalul- hazana in shi/ta sahla",
    "1- La ba/sa tahoorun in shaal-lah",
    "2- Asalul-lahal-AAatheem rabbal-AAarshil-AAatheem an yashfeek",
    "Inna lillahi wa-inna ilayhi rajiAAoon, allahumma/- jurnee fee museebatee wakhluf lee khayran minha",
    AzkarImgLogo,//"Allahummagh-fir lahu warhamh, waAAafihi, waAAfu AAanh, wa-akrim nuzulah, wawassiAA mudkhalah, waghsilhu bilma-i waththalji walbarad, wanaqqihi minal-khataya kama naqqaytath-thawbal-abyada minad-danas, wa-abdilhu daran khayran min darih, wa-ahlan khayran min ahlih wazawjan khayran min zawjih, wa-adkhilhul-jannah, wa-aAAithhu min AAathabil-qabr, waAAathabin-nar",
    "Assalamu AAalaykum ahlad-diyari minal-mu/mineena walmuslimeen, wa-inna in shaal-lahu bikum lahiqoon, nas-alul-laha lana walakumul-Aaafiyah",
    "aAAoothu billahi minash-shaytanir-rajeem.",
    "Barakal-lahu laka fee ahlika wamalik",
    "La ilaha illal-lah, wahdahu la shareeka lah, lahul-mulku walahul-hamd, yuhyee wayumeetu wahuwa hayyun la yamoot, biyadihil-khayru wahuwa AAala kulli shayin qadeer",
    "When he used to receive pleasant news, he would say:\n\nAlhamdu lillahil-lathee biniAAmatihi tatimmus-salihat",
    "Upon receiving displeasing news, he would say:\n\nAlhamdu lillahi AAala kulli hal",
};

//Prayer 音译
const char azkar_Pra_tr[Azkar_Pra_Sec][240] = 
{
    "Subhanakal-lahumma wabihamdika watabarakas-muka wataAAala jadduka wala ilaha ghayruk",
    "Read Surat Al-Fatihah and a small Surah",
    "Subhana rabbiyal-AAatheem",
    "SamiAAal-lahu liman hamidah",
    "Subhana rabbiyal-aAAla ",
    "Rabbigh-fir lee, rabbigh-fir lee",
    "Attahiyyatu lillahi wassalawatu wattayyibat, assalamu AAalayka ayyuhan-nabiyyu warahmatul-lahi wabarakatuh, assalamu AAalayna waAAala AAibadil-lahis-saliheen. Ash-hadu an la ilaha illal-lah, wa-ashhadu anna Muhammadan AAabduhu warasooluh",
    AzkarImgLogo,//"Attahiyyatu lillahi wassalawatu wattayyibat, assalamu AAalayka ayyuhan-nabiyyu warahmatul-lahi wabarakatuh, assalamu AAalayna waAAala AAibadil-lahis-saliheen. Ash-hadu an la ilaha illal-lah, wa-ashhadu anna Muhammadan AAabduhu warasooluh. Allahumma salli AAala Muhammad, wa-AAala ali Muhammad, kama sallayta AAala Ibraheema wa-AAala ali Ibraheem, innaka Hameedun Majeed, allahumma barik AAala Muhammad, wa-AAala ali Muhammad, kama barakta AAala Ibraheema wa-AAala ali Ibraheem, innaka Hameedun Majeed",
    "Allahumma innee aAAoothu bika min AAathabil-qabr, wamin AAathabi jahannam, wamin fitnatil-mahya walmamat, wamin shari fitnatil-maseehid-dajjal",
};

/******************************祈祷词音译end***************************************/



/******************************祈祷词阿拉伯start************************************/

const char azkar_MR_ar[Azkar_MR_Sec][548] =
{

    AzkarImgLogo,//"أَعُوذُ بِاللَّهِ مِنَ الشَّيطَانِ الرَّجِيمِ \n اللَّهُ لاَ إِلَهَ إِلاَّ هُوَ الْحَيُّ الْقَيُّومُ لاَ تَأْخُذُهُ سِنَةٌ وَلاَ نَوْمٌ لَّهُ مَا فِي السَّمَوَاتِ وَمَا فِي الأَرْضِ مَن ذَا الَّذِي يَشْفَعُ عِنْدَهُ إِلاَّ بِإِذْنِهِ يَعْلَمُ مَا بَيْنَ أَيْدِيهِمْ وَمَا خَلْفَهُمْ وَلاَ يُحِيطُونَ بِشَيْءٍ مِّنْ عِلْمِهِ إِلاَّ بِمَا شَاء وَسِعَ كُرْسِيُّهُ السَّمَوَاتِ وَالْأَرْضَ وَلاَ يَؤُودُهُ حِفْظُهُمَا وَهُوَ الْعَلِيُّ الْعَظِيمُ",
    "بسم الله الرحمن الرحيم \n قُلْ هُوَ اللَّهُ أَحَدٌ* اللَّهُ الصَّمَدُ* لَمْ يَلِدْ وَلَمْ يُولَدْ* وَلَمْ يَكُن لَّهُ كُفُواً أَحَدٌ",
    "بسم الله الرحمن الرحيم\n قُلْ أَعُوذُ بِرَبِّ الْفَلَقِ* مِن شَرِّ مَا خَلَقَ* وَمِن شَرِّ غَاسِقٍ إِذَا وَقَبَ* وَمِن شَرِّ النَّفَّاثَاتِ فِي الْعُقَدِ* وَمِن شَرِّ حَاسِدٍ إِذَا حَسَدَ",
    "بسم الله الرحمن الرحيم \n قُلْ أَعُوذُ بِرَبِّ النَّاسِ* مَلِكِ النَّاسِ* إِلَهِ النَّاسِ* مِن شَرِّ الْوَسْوَاسِ الْخَنَّاسِ* الَّذِي يُوَسْوِسُ فِي صُدُورِ النَّاسِ* مِنَ الْجِنَّةِ وَ النَّاسِ",
    AzkarImgLogo,//"أَصْبَحْنَا وَأَصْبَحَ الْمُلْكُ لِلَّهِ ، وَالْحَمْدُ لِلَّهِ، لاَ إِلَهَ إلاَّ اللَّهُ وَحْدَهُ لاَ شَرِيكَ لَهُ، لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٌ، رَبِّ أَسْأَلُكَ خَيْرَ مَا فِي هَذَا الْيَوْمِ وَخَيرَ مَا بَعْدَهُ ، وَأَعُوذُ بِكَ مِنْ شَرِّ مَا فِي هَذَا الْيَوْمِ وَشَرِّ مَا بَعْدَهُ، رَبِّ أَعُوذُ بِكَ مِنَ الْكَسَلِ وَسُوءِ الْكِبَرِ، رَبِّ أَعُوذُ بِكَ مِنْ عَذَابٍ فِي النَّارِ وَعَذَابٍ فِي الْقَبْرِ",
    "اللَّهُمَّ بِكَ أَصْبَحْنَا، وَبِكَ أَمْسَيْنَا ، وَبِكَ نَحْيَا، وَبِكَ نَمُوتُ وَإِلَيْكَ النُّشُورُ",
    "اللَّهُمَّ أَنْتَ رَبِّي لَا إِلَهَ إِلاَّ أَنْتَ، خَلَقْتَنِي وَأَنَا عَبْدُكَ، وَأَنَا عَلَى عَهْدِكَ وَوَعْدِكَ مَا اسْتَطَعْتُ، أَعُوذُ بِكَ مِنْ شَرِّ مَا صَنَعْتُ، أَبُوءُ لَكَ بِنِعْمَتِكَ عَلَيَّ، وَأَبُوءُ بِذَنْبِي فَاغْفِرْ لِي فَإِنَّهُ لاَ يَغْفِرُ الذُّنوبَ إِلاَّ أَنْتَ",
    "اللَّهُمَّ إِنِّي أَصْبَحْتُ أُشْهِدُكَ، وَأُشْهِدُ حَمَلَةَ عَرْشِكَ، وَمَلاَئِكَتِكَ، وَجَمِيعَ خَلْقِكَ، أَنَّكَ أَنْتَ اللَّهُ لَا إِلَهَ إِلاَّ أَنْتَ وَحْدَكَ لاَ شَرِيكَ لَكَ، وَأَنَّ مُحَمَّداً عَبْدُكَ وَرَسُولُكَ",
    "اللَّهُمَّ مَا أَصْبَحَ بِي مِنْ نِعْمَةٍ أَوْ بِأَحَدٍ مِنْ خَلْقِكَ فَمِنْكَ وَحْدَكَ لاَ شَرِيكَ لَكَ، فَلَكَ الْحَمْدُ وَلَكَ الشُّكْرُ",
    "اللَّهُمَّ عَافِنِي فِي بَدَنِي، اللَّهُمَّ عَافِنِي فِي سَمْعِي، اللَّهُمَّ عَافِنِي فِي بَصَرِي، لاَ إِلَهَ إِلاَّ أَنْتَ. اللَّهُمَّ إِنِّي أَعُوذُ بِكَ مِنَ الْكُفْرِ، وَالفَقْرِ، وَأَعُوذُ بِكَ مِنْ عَذَابِ القَبْرِ، لاَ إِلَهَ إِلاَّ أَنْتَ",
    "حَسْبِيَ اللَّهُ لاَ إِلَهَ إِلاَّ هُوَ عَلَيهِ تَوَكَّلتُ وَهُوَ رَبُّ الْعَرْشِ الْعَظِيمِ",
    AzkarImgLogo,//"اللَّهُمَّ إِنِّي أَسْأَلُكَ الْعَفْوَ وَالْعَافِيَةَ فِي الدُّنْيَا وَالآخِرَةِ، اللَّهُمَّ إِنِّي أَسْأَلُكَ الْعَفْوَ وَالْعَافِيَةَ: فِي دِينِي وَدُنْيَايَ وَأَهْلِي، وَمَالِي، اللَّهُمَّ اسْتُرْ عَوْرَاتِي، وَآمِنْ رَوْعَاتِي، اللَّهُمَّ احْفَظْنِي مِنْ بَينِ يَدَيَّ، وَمِنْ خَلْفِي، وَعَنْ يَمِينِي، وَعَنْ شِمَالِي، وَمِنْ فَوْقِي، وَأَعُوذُ بِعَظَمَتِكَ أَنْ أُغْتَالَ مِنْ تَحْتِي",
    "اللَّهُمَّ عَالِمَ الغَيْبِ وَالشَّهَادَةِ فَاطِرَ السَّمَوَاتِ وَالْأَرْضِ، رَبَّ كُلِّ شَيْءٍ وَمَلِيكَهُ، أَشْهَدُ أَنْ لاَ إِلَهَ إِلاَّ أَنْتَ، أَعُوذُ بِكَ مِنْ شَرِّ نَفْسِي، وَمِنْ شَرِّ الشَّيْطانِ وَشَرَكِهِ، وَأَنْ أَقْتَرِفَ عَلَى نَفْسِي سُوءاً، أَوْ أَجُرَّهُ إِلَى مُسْلِمٍ",
    "بِسْمِ اللَّهِ الَّذِي لاَ يَضُرُّ مَعَ اسْمِهِ شَيْءٌ فِي الْأَرْضِ وَلاَ فِي السّمَاءِ وَهُوَ السَّمِيعُ الْعَلِيمُ",
    "رَضِيتُ بِاللَّهِ رَبَّاً، وَبِالْإِسْلاَمِ دِيناً، وَبِمُحَمَّدٍ صلى الله عليه وسلم نَبِيّاً",
    "يَا حَيُّ يَا قَيُّومُ بِرَحْمَتِكَ أَسْتَغيثُ أَصْلِحْ لِي شَأْنِيَ كُلَّهُ وَلاَ تَكِلْنِي إِلَى نَفْسِي طَرْفَةَ عَيْنٍ",
    "أَصْبَحْنَا وَأَصْبَحَ الْمُلْكُ لِلَّهِ رَبِّ الْعَالَمِينَ، اللَّهُـمَّ إِنِّي أَسْأَلُكَ خَيْرَ هَذَا الْيَوْمِ:فَتْحَهُ، وَنَصْرَهُ، وَنورَهُ، وَبَرَكَتَهُ، وَهُدَاهُ، وَأَعُوذُ بِكَ مِنْ شَرِّ مَا فِيهِ وَشَرِّ مَا بَعْدَهُ",
    "أَصْبَحْنا عَلَى فِطْرَةِ الْإِسْلاَمِ، وَعَلَى كَلِمَةِ الْإِخْلاَصِ، وَعَلَى دِينِ نَبِيِّنَا مُحَمَّدٍ صلى الله عليه وسلم، وَعَلَى مِلَّةِ أَبِينَا إِبْرَاهِيمَ، حَنِيفاً مُسْلِماً وَمَا كَانَ مِنَ الْمُشرِكِينَ",
    "سُبْحَانَ اللَّهِ وَبِحَمْدِهِ",
    "لاَ إِلَهَ إِلاَّ اللَّهُ وَحْدَهُ لاَ شَرِيكَ لَهُ، لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ، وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٌ",
    "لاَ إِلَهَ إِلاَّ اللَّهُ، وَحْدَهُ لاَ شَرِيكَ لَهُ، لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٌ",
    "سُبْحَانَ اللَّهِ وَبِحَمْدِهِ: عَدَدَ خَلْقِهِ، وَرِضَا نَفْسِهِ، وَزِنَةَ عَرْشِهِ، وَمِدَادَ كَلِمَاتِهِ",
    "اللَّهُمَّ إِنِّي أَسْأَلُكَ عِلْماً نَافِعاً، وَرِزْقاً طَيِّباً، وَعَمَلاً مُتَقَبَّلاً",
    "أَسْتَغْفِرُ اللَّهَ وَأَتُوبُ إِلَيْهِ",
    "اللَّهُمَّ صَلِّ وَسَلِّمْ عَلَى نَبَيِّنَا مُحَمَّدٍ",
};

const char azkar_ER_ar[Azkar_ER_Sec][548] = 
{
    AzkarImgLogo,//"أَعُوذُ بِاللَّهِ مِنَ الشَّيطَانِ الرَّجِيمِ \n اللَّهُ لاَ إِلَهَ إِلاَّ هُوَ الْحَيُّ الْقَيُّومُ لاَ تَأْخُذُهُ سِنَةٌ وَلاَ نَوْمٌ لَّهُ مَا فِي السَّمَوَاتِ وَمَا فِي الأَرْضِ مَن ذَا الَّذِي يَشْفَعُ عِنْدَهُ إِلاَّ بِإِذْنِهِ يَعْلَمُ مَا بَيْنَ أَيْدِيهِمْ وَمَا خَلْفَهُمْ وَلاَ يُحِيطُونَ بِشَيْءٍ مِّنْ عِلْمِهِ إِلاَّ بِمَا شَاء وَسِعَ كُرْسِيُّهُ السَّمَوَاتِ وَالْأَرْضَ وَلاَ يَؤُودُهُ حِفْظُهُمَا وَهُوَ الْعَلِيُّ الْعَظِيمُ",
    "بسم الله الرحمن الرحيم \n قُلْ هُوَ اللَّهُ أَحَدٌ* اللَّهُ الصَّمَدُ* لَمْ يَلِدْ وَلَمْ يُولَدْ* وَلَمْ يَكُن لَّهُ كُفُواً أَحَدٌ",
    "بسم الله الرحمن الرحيم\n قُلْ أَعُوذُ بِرَبِّ الْفَلَقِ* مِن شَرِّ مَا خَلَقَ* وَمِن شَرِّ غَاسِقٍ إِذَا وَقَبَ* وَمِن شَرِّ النَّفَّاثَاتِ فِي الْعُقَدِ* وَمِن شَرِّ حَاسِدٍ إِذَا حَسَدَ",
    "بسم الله الرحمن الرحيم \n قُلْ أَعُوذُ بِرَبِّ النَّاسِ* مَلِكِ النَّاسِ* إِلَهِ النَّاسِ* مِن شَرِّ الْوَسْوَاسِ الْخَنَّاسِ* الَّذِي يُوَسْوِسُ فِي صُدُورِ النَّاسِ* مِنَ الْجِنَّةِ وَ النَّاسِ",
    AzkarImgLogo,//"أَمْسَيْنَا وَأَمْسَى الْمُلْكُ لِلَّهِ، وَالْحَمْدُ لِلَّهِ، لاَ إِلَهَ إلاَّ اللَّهُ وَحْدَهُ لاَ شَرِيكَ لَهُ، لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٌ، رَبِّ أَسْأَلُكَ خَيْرَ مَا فِي هَذِهِ اللَّيْلَةِ وَخَيْرَ مَا بَعْدَهَا، وَأَعُوذُ بِكَ مِنْ شَرِّ مَا فِي هَذِهِ اللَّيْلَةِ وَشَرِّ مَا بَعْدَهَا، رَبِّ أَعُوذُ بِكَ مِنَ الْكَسَلِ وَسُوءِ الْكِبَرِ، رَبِّ أَعُوذُ بِكَ مِنْ عَذَابٍ فِي النَّارِ وَعَذَابٍ فِي الْقَبْرِ",
    "أَعُوذُ بِكَلِمَاتِ اللَّهِ التَّامَّاتِ مِنْ شَرِّ مَا خَلَقَ",
    "اللّهُـمَّ بِكَ أَمْسَـينا، وَبِكَ أَصْـبَحْنا، وَبِكَ نَحْـيا، وَبِكَ نَمـوتُ وَإِلَـيْكَ المَصـير",
    "اللَّهُمَّ عَافِنِي فِي بَدَنِي، اللَّهُمَّ عَافِنِي فِي سَمْعِي، اللَّهُمَّ عَافِنِي فِي بَصَرِي، لاَ إِلَهَ إِلاَّ أَنْتَ. اللَّهُمَّ إِنِّي أَعُوذُ بِكَ مِنَ الْكُفْرِ، وَالفَقْرِ، وَأَعُوذُ بِكَ مِنْ عَذَابِ القَبْرِ، لاَ إِلَهَ إِلاَّ أَنْتَ",
    "اللَّهُمَّ عَالِمَ الغَيْبِ وَالشَّهَادَةِ فَاطِرَ السَّمَوَاتِ وَالْأَرْضِ، رَبَّ كُلِّ شَيْءٍ وَمَلِيكَهُ، أَشْهَدُ أَنْ لاَ إِلَهَ إِلاَّ أَنْتَ، أَعُوذُ بِكَ مِنْ شَرِّ نَفْسِي، وَمِنْ شَرِّ الشَّيْطانِ وَشَرَكِهِ، وَأَنْ أَقْتَرِفَ عَلَى نَفْسِي سُوءاً، أَوْ أَجُرَّهُ إِلَى مُسْلِمٍ",
    "بِسْمِ اللَّهِ الَّذِي لاَ يَضُرُّ مَعَ اسْمِهِ شَيْءٌ فِي الْأَرْضِ وَلاَ فِي السّمَاءِ وَهُوَ السَّمِيعُ الْعَلِيمُ",
    "رَضِيتُ بِاللَّهِ رَبَّاً، وَبِالْإِسْلاَمِ دِيناً، وَبِمُحَمَّدٍ صلى الله عليه وسلم نَبِيّاً",
    "يَا حَيُّ يَا قَيُّومُ بِرَحْمَتِكَ أَسْتَغيثُ أَصْلِحْ لِي شَأْنِيَ كُلَّهُ وَلاَ تَكِلْنِي إِلَى نَفْسِي طَرْفَةَ عَيْنٍ",
    "سُبْحَانَ اللَّهِ وَبِحَمْدِهِ",
    "لاَ إِلَهَ إِلاَّ اللَّهُ وَحْدَهُ لاَ شَرِيكَ لَهُ، لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ، وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٌ",
};

const char azkar_Apr_ar[Azkar_APR_Sec][504] = 
{
    "اللهُ أكبرُ",
    "استغفر الله، استغفر الله ، استغفر الله",
    "اللهم أنت السلام ومنك السلام تباركت يا ذا الجلال والإكرام",
    "اللهم أعني على ذكرك وشكرك وحسن عبادتك",
    "سبحانَ اللهِ",
    "الحمدُ للهِ",
    "اللهُ أكبرُ",
    "لا اله الا الله وحده لا شريك له له الملك وله الحمد وهو على كل شيء قدير",
    "اللهم اني أسألك رضاك وَالجَنَّةَ وأعوذ بك من سخطك وَالنَّارِ",
    AzkarImgLogo,//"أَعُوذُ بِاللَّهِ مِنَ الشَّيطَانِ الرَّجِيمِ \n اللَّهُ لاَ إِلَهَ إِلاَّ هُوَ الْحَيُّ الْقَيُّومُ لاَ تَأْخُذُهُ سِنَةٌ وَلاَ نَوْمٌ لَّهُ مَا فِي السَّمَوَاتِ وَمَا فِي الأَرْضِ مَن ذَا الَّذِي يَشْفَعُ عِنْدَهُ إِلاَّ بِإِذْنِهِ يَعْلَمُ مَا بَيْنَ أَيْدِيهِمْ وَمَا خَلْفَهُمْ وَلاَ يُحِيطُونَ بِشَيْءٍ مِّنْ عِلْمِهِ إِلاَّ بِمَا شَاء وَسِعَ كُرْسِيُّهُ السَّمَوَاتِ وَالْأَرْضَ وَلاَ يَؤُودُهُ حِفْظُهُمَا وَهُوَ الْعَلِيُّ الْعَظِيمُ",
    "بسم الله الرحمن الرحيم \n قُلْ هُوَ اللَّهُ أَحَدٌ* اللَّهُ الصَّمَدُ* لَمْ يَلِدْ وَلَمْ يُولَدْ* وَلَمْ يَكُن لَّهُ كُفُواً أَحَدٌ",
    "بسم الله الرحمن الرحيم\n قُلْ أَعُوذُ بِرَبِّ الْفَلَقِ* مِن شَرِّ مَا خَلَقَ* وَمِن شَرِّ غَاسِقٍ إِذَا وَقَبَ* وَمِن شَرِّ النَّفَّاثَاتِ فِي الْعُقَدِ* وَمِن شَرِّ حَاسِدٍ إِذَا حَسَدَ",
    "بسم الله الرحمن الرحيم \n قُلْ أَعُوذُ بِرَبِّ النَّاسِ* مَلِكِ النَّاسِ* إِلَهِ النَّاسِ* مِن شَرِّ الْوَسْوَاسِ الْخَنَّاسِ* الَّذِي يُوَسْوِسُ فِي صُدُورِ النَّاسِ* مِنَ الْجِنَّةِ وَ النَّاسِ",
    "اللّهُـمَّ صَلِّ عَلـى مُحمَّـد، وَعَلـى آلِ مُحمَّد، كَمـا صَلَّيـتَ عَلـى إبْراهـيمَ وَعَلـى آلِ إبْراهـيم، إِنَّكَ حَمـيدٌ مَجـيد ، اللّهُـمَّ بارِكْ عَلـى مُحمَّـد، وَعَلـى آلِ مُحمَّـد، كَمـا بارِكْتَ عَلـى إبْراهـيمَ وَعَلـى آلِ إبْراهيم، إِنَّكَ حَمـيدٌ مَجـيد",
};

const char azkar_Hau_ar[Azkar_HAU_Sec][484] =
{
    "لَبَّيْكَ اللهُمَّ لَبَّيْكَ – لَبَّيْكَ لَا شَرِيْكَ لَكَ لَبَّيْكَ – إِنَّ الْحَمْدَ وَالنِّعْمَةَ لَكَ وَالْمُلْكَ – لَا شَرِيْكَ لَكَ –",
    "اللهُ أَكْـبَر",
    "اللهُ أكْبَرُ، اللهُ أَكْبَرُ، اللهُ أَكْبَرُ\n \n لَا إِلهَ إِلَّا اللهُ وَحْدَهُ لَا شَرِيكَ لَهُ، لَهُ المُلْكُ وَلَهُ الحَمْدُ وهُوَ عَلى كُلِّ شَيءٍ قَديرٌ، لَا إِلَهَ إِلَّا اللهُ وَحْدَهُ أَنْجَزَ وَعْدَهُ، وَنَصَرَ عَبْدَهُ وَهَزَمَ الأَحْزَابَ وَحْدَهُ",
    "اللهُ أَكْـبَر",
    "أفضل الدعاء يوم عرفة:\n لَا إِلَهَ إِلَّا اللَّهُ وَحْدَهُ لَا شَرِيكَ لَهُ لَهُ الْمُلْكُ وَلَهُ الْحَمْدُ وَهُوَ عَلَى كُلِّ شَيْءٍ قَدِيرٍ",
};

const char azkar_Fas_ar[Azkar_Fas_Sec][308] = 
{
    "بِصَوْمِ غَدٍ نَوَيْتُ مِنْ شَهْرِ رَمَضَانَ",
    "ذَهَـبَ الظَّمَـأُ، وَابْتَلَّـتِ العُـروق، وَثَبَـتَ الأجْـرُ إِنْ شـاءَ الله",
    "اللهُ أَكْـبَر، اللّهُمَّ أَهِلَّـهُ عَلَيْـنا بِالأمْـنِ وَالإيمـان، والسَّلامَـةِ والإسْلام، وَالتَّـوْفيـقِ لِما تُحِـبُّ وَتَـرْضـى، رَبُّنـا وَرَبُّكَ الله",
    "إِنِّي صَائِمٌ، إِنِّي صَائِمٌ",
};

const char azkar_Wwu_ar[Azkar_WWU_Sec][388] = 
{
    "الحَمْـدُ لِلّهِ الّذي أَحْـيانا بَعْـدَ ما أَماتَـنا وَإليه النُّـشور",
    "لا إلهَ إلاّ اللّهُ وَحْـدَهُ لا شَـريكَ له، لهُ المُلـكُ ولهُ الحَمـد، وهوَ على كلّ شيءٍ قدير، سُـبْحانَ اللهِ، والحمْـدُ لله ، ولا إلهَ إلاّ اللهُ واللهُ أكبَر، وَلا حَولَ وَلا قوّة إلاّ باللّهِ العليّ العظيم",
    "الحمدُ للهِ الذي عافاني في جَسَدي وَرَدّ عَليّ روحي وَأَذِنَ لي بِذِكْرِه",
};

const char azkar_Gar_ar[Azkar_Gar_Sec][300] = 
{
    "الحمدُ للهِ الّذي كَساني هذا (الثّوب) وَرَزَقَنيه مِنْ غَـيـْرِ حَولٍ مِنّي وَلا قـوّة",
    "اللّهُـمَّ لَـكَ الحَـمْـدُ أنْـتَ كَسَـوْتَنيهِ، أَسْأَلُـكَ مِـنْ خَـيرِهِ وَخَـيْرِ مَا صُنِعَ لَـه، وَأَعوذُ بِكَ مِـنْ شَـرِّهِ وَشَـرِّ مـا صُنِعَ لَـهُ",
    "بِسْمِ الله",
};

const char azkar_Abu_ar[Azkar_Abu_Sec][212] = 
{
    "بِسْمِ الله",
    "أَشْهَدُ أَنْ لا إِلَـهَ إِلاّ اللهُ وَحْدَهُ لا شَريـكَ لَـهُ وَأَشْهَدُ أَنَّ مُحَمّـداً عَبْـدُهُ وَرَسـولُـه",
    "اللّهُـمَّ اجْعَلنـي مِنَ التَّـوّابينَ وَاجْعَـلْني مِنَ المتَطَهّـرين",
};

const char azkar_Home_ar[Azkar_Home_Sec][212] = 
{
    "بِسْـمِ اللهِ وَلَجْنـا، وَبِسْـمِ اللهِ خَـرَجْنـا، وَعَلـى رَبِّنـا تَوَكّلْـنا",
    "بِسْمِ اللهِ ، تَوَكَّلْـتُ عَلى اللهِ وَلا حَوْلَ وَلا قُـوَّةَ إِلاّ بِالله",
    "بِسْمِ الله اللّهُـمَّ إِنِّـي أَعـوذُ بِـكَ مِـنَ الْخُـبْثِ وَالْخَبائِث",
    "غُفْـرانَك",
    "بِسْمِ الله",
    "أَشْهَدُ أَنْ لا إِلَـهَ إِلاّ اللهُ وَحْدَهُ لا شَريـكَ لَـهُ وَأَشْهَدُ أَنَّ مُحَمّـداً عَبْـدُهُ وَرَسـولُـه",
    "اللّهُـمَّ اجْعَلنـي مِنَ التَّـوّابينَ وَاجْعَـلْني مِنَ المتَطَهّـرين",
};

const char azkar_Mos_ar[Azkar_Mos_Sec][440] = 
{
    "اللّهُـمَّ اجْعَـلْ في قَلْبـي نورا ، وَفي لِسـاني نورا، وَاجْعَـلْ في سَمْعي نورا، وَاجْعَـلْ في بَصَري نورا، وَاجْعَـلْ مِنْ خَلْفي نورا، وَمِنْ أَمامـي نورا، وَاجْعَـلْ مِنْ فَوْقـي نورا ، وَمِن تَحْتـي نورا اللّهُـمَّ أَعْطِنـي نورا",
    "أَعوذُ باللهِ العَظيـم وَبِوَجْهِـهِ الكَرِيـم وَسُلْطـانِه القَديـم مِنَ الشّيْـطانِ الرَّجـيم، بِسْـمِ الله، وَالصَّلاةُ وَالسَّلامُ عَلى رَسولِ الله، اللّهُـمَّ افْتَـحْ لي أَبْوابَ رَحْمَتـِك",
    "بِسمِ الله وَالصّلاةُ وَالسّلامُ عَلى رَسولِ الله، اللّهُـمَّ إِنّـي أَسْأَلُكَ مِـنْ فَضْـلِك، اللّهُـمَّ اعصِمْنـي مِنَ الشَّيْـطانِ الرَّجـيم",
};

const char azkar_Ath_ar[Azkar_Ath_Sec][364] = 
{
    "يقول من سمع الأذان كما قال المؤذن إلا في \"حي على الصلاة وحي على الفلاح \" فإنه يقول \"لاحول ولا قوة إلا بالله \"",
    "اللّهُـمَّ رَبَّ هَذِهِ الدّعْـوَةِ التّـامَّة وَالصّلاةِ القَـائِمَة آتِ محَـمَّداً الوَسيـلةَ وَالْفَضـيلَة وَابْعَـثْه مَقـامـاً مَحـموداً الَّذي وَعَـدْتَه إِنَّـكَ لا تُـخْلِفُ الميـعاد",
};

const char azkar_Food_ar[Azkar_Food_Sec][176] =
{
    "بِسْمِ الله",
    "الْحَمْـدُ للهِ الَّذي أَطْعَمَنـي هـذا وَرَزَقَنـيهِ مِنْ غَـيْرِ حَوْلٍ مِنِّي وَلا قُوَّة",
    "اللّهُـمَّ بارِكْ لَهُمْ فيما رَزَقْـتَهُم، وَاغْفِـرْ لَهُـمْ وَارْحَمْهُمْ",
};

const char azkar_Tra_ar[Azkar_Tra_Sec][132] = 
{
    AzkarImgLogo,//"اللهُ أكبَر ، اللهُ أكبَر ، اللهُ أكبَر، سُـبْحانَ الَّذي سَخَّـرَ لَنا هذا وَما كُنّا لَهُ مُقْـرِنين، وَإِنّا إِلى رَبِّنـا لَمُنْقَـلِبون، اللّهُـمَّ إِنّا نَسْـأَلُكَ في سَفَـرِنا هذا البِـرَّ وَالتَّـقْوى، وَمِنَ الْعَمَـلِ ما تَـرْضى، اللّهُـمَّ هَوِّنْ عَلَـينا سَفَرَنا هذا وَاطْوِ عَنّا بُعْـدَه، اللّهُـمَّ أَنْـتَ الصّـاحِبُ في السَّـفَر، وَالْخَلـيفَةُ في الأهـلِ، اللّهُـمَّ إِنّـي أَعـوذُبِكَ مِنْ وَعْـثاءِ السَّـفَر، وَكَآبَةِ الْمَنْـظَر، وَسوءِ الْمُنْـقَلَبِ في المـالِ وَالأَهْـل\n وعند الرجوع من السفر يزيد عليها \n \n آيِبـونَ تائِبـونَ عابِـدونَ لِرَبِّـنا حـامِـدون",
    "أَسْتَـوْدِعُكُـمُ اللَّهَ الَّذي لا تَضـيعُ وَدائِعُـه",
    "أَسْتَـوْدِعُ اللَّهَ ديـنَكَ وَأَمانَتَـكَ، وَخَـواتيـمَ عَمَـلِك",
};

const char azkar_OR_ar[Azkar_OR_Sec][580] = 
{
    "ثبت عن رسول الله عليه الصلاة والسلام أنه قال: الرؤيا الصالحة من الله والحلم من الشيطان، فإذا رأى أحدكم ما يكره فلينفث عن يساره ثلاثًا وليستعذ بالله من الشيطان ومن شر ما رأى ثلاثًا ثم لينقلب على جنبه الآخر فإنها لا تضره ولا يخبر بها أحدًا",
    "اللّهُـمَّ اهْـدِنـي فـيمَنْ هَـدَيْـت، وَعـافِنـي فـيمَنْ عافَـيْت، وَتَوَلَّـني فـيمَنْ تَوَلَّـيْت ، وَبارِكْ لـي فـيما أَعْطَـيْت، وَقِـني شَرَّ ما قَضَـيْت، فَإِنَّـكَ تَقْـضي وَلا يُقْـضى عَلَـيْك ، إِنَّـهُ لا يَـذِلُّ مَنْ والَـيْت، [ وَلا يَعِـزُّ مَن عـادَيْت ]، تَبـارَكْـتَ رَبَّـنا وَتَعـالَـيْت",
    AzkarImgLogo,//"اللّهُـمَّ إِنِّي عَبْـدُكَ ابْنُ عَبْـدِكَ ابْنُ أَمَتِـكَ نَاصِيَتِي بِيَـدِكَ، مَاضٍ فِيَّ حُكْمُكَ، عَدْلٌ فِيَّ قَضَاؤكَ أَسْأَلُـكَ بِكُلِّ اسْمٍ هُوَ لَكَ سَمَّـيْتَ بِهِ نَفْسَكَ أِوْ أَنْزَلْتَـهُ فِي كِتَابِكَ، أَوْ عَلَّمْـتَهُ أَحَداً مِنْ خَلْقِـكَ أَوِ اسْتَـأْثَرْتَ بِهِ فِي عِلْمِ الغَيْـبِ عِنْـدَكَ أَنْ تَجْـعَلَ القُرْآنَ رَبِيـعَ قَلْبِـي، وَنورَ صَـدْرِي وجَلَاءَ حُـزْنِي وذَهَابَ هَمِّـي",
    " اللّهُـمَّ إِنِّي أَعْوذُ بِكَ مِنَ الهَـمِّ وَ الْحُـزْنِ، والعًجْـزِ والكَسَلِ والبُخْـلِ والجُـبْنِ، وضَلْـعِ الـدَّيْنِ وغَلَبَـةِ الرِّجال",
    "اللّهُـمَّ لا سَـهْلَ إِلاّ ما جَعَلـتَهُ سَهـلاً، وَأَنْتَ تَجْـعَلُ الْحَـزَنَ إِذا شِـئْتَ سَهـْلاً",
    "1- لا بأْسَ طَهـورٌ إِنْ شـاءَ الله ",
    " 2- أَسْـأَلُ اللهَ العَـظيـم، رَبَّ العَـرْشِ العَـظيـم أَنْ يَشْفـيك",
    "إِنّا للهِ وَإِنَا إِلَـيْهِ راجِعـون ، اللهُـمِّ اْجُـرْني في مُصـيبَتي، وَاخْلُـفْ لي خَيْـراً مِنْـها",
    AzkarImgLogo,//"اللهُـمِّ اغْفِـرْ لَهُ وَارْحَمْـه ، وَعافِهِ وَاعْفُ عَنْـه ، وَأَكْـرِمْ نُزُلَـه ، وَوَسِّـعْ مُدْخَـلَه ، وَاغْسِلْـهُ بِالْمـاءِ وَالثَّـلْجِ وَالْبَـرَدْ ، وَنَقِّـهِ مِنَ الْخطـايا كَما نَـقّيْتَ الـثَّوْبُ الأَبْيَـضُ مِنَ الدَّنَـسْ ، وَأَبْـدِلْهُ داراً خَـيْراً مِنْ دارِه ، وَأَهْلاً خَـيْراً مِنْ أَهْلِـه ، وَزَوْجَـاً خَـيْراً مِنْ زَوْجِه ، وَأَدْخِـلْهُ الْجَـنَّة ، وَأَعِـذْهُ مِنْ عَذابِ القَـبْر وَعَذابِ النّـار",
    "السَّلامُ عَلَـيْكُمْ أَهْلَ الدِّيارِ مِنَ المؤْمِنيـنَ وَالْمُسْلِمين، وَإِنّا إِنْ شاءَ اللهُ بِكُـمْ لاحِقـون، نَسْـاَلُ اللهَ لنـا وَلَكُـمْ العـافِيَة",
    "أَعـوذُ بِاللهِ مِنَ الشَّيْـطانِ الرَّجيـم",
    "بارَكَ اللهُ لَكَ في أَهْلِكَ وَمالِك",
    "لا إلهَ إلاّ اللّه وحدَهُ لا شريكَ لهُ، لهُ المُلْـكُ ولهُ الحَمْـد، يُحْيـي وَيُميـتُ وَهُوَ حَيٌّ لا يَمـوت، بِيَـدِهِ الْخَـيْرُ وَهوَ على كلّ شيءٍ قدير",
    "عند تلقي خبر مفرح يقول\n الْحَمْـدُ للهِ الَّذي بِنِـعْمَتِهِ تَتِـمُّ الصّـالِحات ",
    " عند تلقي خبر محزن يقول\n الْحَمْـدُ للهِ على كُـلِّ حال",
};

const char azkar_Pra_ar[Azkar_Pra_Sec][504] = 
{
    "سُبْـحانَكَ اللّهُـمَّ وَبِحَمْـدِكَ وَتَبارَكَ اسْمُـكَ وَتَعـالى جَـدُّكَ وَلا إِلهَ غَيْرُك ",
    "قراءة سورة الفاتحة وسورة صغيرة",
    "سُبْـحانَ رَبِّـيَ الْعَظـيم ",
    "سَمِـعَ اللهُ لِمَـنْ حَمِـدَه ",
    "سُبْـحانَ رَبِّـيَ الأَعْلـى ",
    "رَبِّ اغْفِـرْ لي ، رَبِّ اغْفِـرْ لي ",
    "التَّحِيّـاتُ للهِ وَالصَّلَـواتُ والطَّيِّـبات ، السَّلامُ عَلَيـكَ أَيُّهـا النَّبِـيُّ وَرَحْمَـةُ اللهِ وَبَرَكـاتُه ، السَّلامُ عَلَيْـنا وَعَلـى عِبـادِ كَ الصَّـالِحـين . أَشْـهَدُ أَنْ لا إِلـهَ إِلاّ الله ، وَأَشْـهَدُ أَنَّ مُحَمّـداً عَبْـدُهُ وَرَسـولُه ",
    AzkarImgLogo,//"التَّحِيّـاتُ للهِ وَالصَّلَـواتُ والطَّيِّـبات ، السَّلامُ عَلَيـكَ أَيُّهـا النَّبِـيُّ وَرَحْمَـةُ اللهِ وَبَرَكـاتُه ، السَّلامُ عَلَيْـنا وَعَلـى عِبـادِ كَ الصَّـالِحـين . أَشْـهَدُ أَنْ لا إِلـهَ إِلاّ الله ، وَأَشْـهَدُ أَنَّ مُحَمّـداً عَبْـدُهُ وَرَسـولُه . اللّهُـمَّ صَلِّ عَلـى مُحمَّـد، وَعَلـى آلِ مُحمَّد، كَمـا صَلَّيـتَ عَلـىإبْراهـيمَ وَعَلـى آلِ إبْراهـيم، إِنَّكَ حَمـيدٌ مَجـيد ، اللّهُـمَّ بارِكْ عَلـى مُحمَّـد، وَعَلـى آلِ مُحمَّـد، كَمـا بارِكْتَ عَلـىإبْراهـيمَ وَعَلـى آلِ إبْراهيم، إِنَّكَ حَمـيدٌ مَجـيد ",
    "اللّهُـمَّ إِنِّـي أَعـوذُ بِكَ مِـنْ عَذابِ القَـبْر، وَمِـنْ عَذابِ جَهَـنَّم، وَمِـنْ فِتْـنَةِ المَحْـيا وَالمَمـات، وَمِـنْ شَـرِّ فِتْـنَةِ المَسيحِ الدَّجّال ",
};

/******************************祈祷词阿拉伯end************************************/


/******************************祈祷词朗读次数************************************/
const u8 azkar_MR_freq[Azkar_MR_Sec] = 
{
    1, 3, 3, 3, 1, 1, 1, 4, 1, 3, 7, 1, 1, 3, \
    3, 1, 1, 1, 100, 10, 100, 3, 1, 100, 10,  
};

const u8 azkar_ER_freq[Azkar_ER_Sec] = 
{
    1, 3, 3, 3, 1, 3, 1, 3, 1, 3, 3, 1, 100, 10,  
};

const u8 azkar_Apr_freq[Azkar_APR_Sec] = 
{
    1, 3, 1, 1, 33, 33, 33, 1, 1, 1, 3, 3, 3, 1,   
};

const u8 azkar_Hau_freq[Azkar_HAU_Sec] = 
{
    1, 1, 1, 1, 1,   
};

const u8 azkar_Fas_freq[Azkar_Fas_Sec] = 
{
    1, 1, 1, 1,    
};

const u8 azkar_Wwu_freq[Azkar_WWU_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_Gar_freq[Azkar_Gar_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_Abu_freq[Azkar_Abu_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_Home_freq[Azkar_Home_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 
};

const u8 azkar_Mos_freq[Azkar_Mos_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_Ath_freq[Azkar_Ath_Sec] = 
{
    1, 1,
};

const u8 azkar_Food_freq[Azkar_Food_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_Tra_freq[Azkar_Tra_Sec] = 
{
    1, 1, 1,
};

const u8 azkar_OR_freq[Azkar_OR_Sec] = 
{
    1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

const u8 azkar_Pra_freq[Azkar_Pra_Sec] = 
{
    1, 1, 3, 1, 3, 1, 1, 1, 1 
};

/******************************祈祷词标题英文音译************************************/
const char azkar_title_MR_en_tr[Azkar_MR_Sec][5] =
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_ER_en_tr[Azkar_ER_Sec][5] =
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_Apr_en_tr[Azkar_APR_Sec][5] =
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_Hau_en_tr[Azkar_HAU_Sec][45] =
{
    "Talbyah",
    "Takbeer when passing the black stone",
    "When at Mount Safa and Mount Marwah",
    "When Throwing stones at Aljamaraat",
    "At Arafat",
};

const char azkar_title_Fas_en_tr[Azkar_Fas_Sec][40] =
{
    "suhoor/sehri (pre dawn meal)",
    "fatoor/iftar (opening the fast)",
    "dua for sighting of the moon",
    "When insulted while \nfasting",
};

const char azkar_title_Wwu_en_tr[Azkar_WWU_Sec][5] =
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_Gar_en_tr[Azkar_Gar_Sec][30] =
{
    "when wearing a garment",
    AzkarNullLogo,
    "Before undressing",
};

const char azkar_title_Abu_en_tr[Azkar_Abu_Sec][30] =
{
    "When starting ablution",
    "Upon completing the ablution",
    AzkarNullLogo,
};

const char azkar_title_Home_en_tr[Azkar_Home_Sec][30] =
{
    "Upon entering the home",
    "When leaving the home",
    "Before entering the toilet",
    "After leaving the toilet",
    "When starting ablution",
    "Upon completing the ablution",
    AzkarNullLogo,
};

const char azkar_title_Mos_en_tr[Azkar_Mos_Sec][40] =
{
    "Supplication when going to the mosque",
    "Upon entering the \nmosque",
    "Upon leaving the \nmosque",
};

const char azkar_title_Ath_en_tr[Azkar_Ath_Sec][25] =
{
    "When hearing the \nAthan",
    "Follwing the Athan",
};

const char azkar_title_Food_en_tr[Azkar_Food_Sec][40] =
{
    "Supplication before eating",
    "Upon completing the meal",
    "Supplication of the guest for the host",
};

const char azkar_title_Tra_en_tr[Azkar_Tra_Sec][50] =
{
    "Supplication for \ntravel",
    "Supplication of the traveler for the resident",
    "Supplication of the resident for the traveler",
};

const char azkar_title_OR_en_tr[Azkar_OR_Sec][55] =
{
    "Upon seeing a bad \ndream",
    "Qunoot Al-Witr",
    "Supplication for anxiety and sorrow",
    AzkarNullLogo,
    "Supplication for whose affairs became difficult",
    "When visiting the sick",
    AzkarNullLogo,
    "Supplication for one afflicted by a calamity",
    "Supplication for deceased at funeral prayer",
    "Visiting the graves",
    "When angry",
    "Supplication said to who offered you of his wealth",
    "When entering the \nmarket",
    "Upon receiving pleasing or displeasing news",
    AzkarNullLogo,
};

const char azkar_title_Pra_en_tr[Azkar_Pra_Sec][60] =
{
    "Supplication at prayer (after takbeer)",
    "While standing",
    "While bowing in prayer (rukooAA)",
    "Upon rising from the bowing posision",
    "Supplication whilst prostrating (sujood)",
    "Supplication between the two prostrations",
    "Tashahhud: what to say in sitting position in prayer",
    "Last Tashahhud: what to say in last sitting position",
    "Supplication before \nsalam",
};

/******************************祈祷词标题阿拉伯************************************/
const char azkar_title_MR_ar[Azkar_MR_Sec][5] = 
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_ER_ar[Azkar_ER_Sec][5] = 
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_Apr_ar[Azkar_APR_Sec][5] = 
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,  
};

const char azkar_title_Hau_ar[Azkar_HAU_Sec][65] = 
{
    "التلبية",
    "التكبير عند المرور بالحجر الأسود",
    "الدعاء عند الصفا والمروة",
    "عند رمي الجمرات",
    "في عرفات",
};

const char azkar_title_Fas_ar[\
    Azkar_Fas_Sec][40] = 
{
    "السحور",
    "الإفطار",
    "رؤية الهلال",
    "أن سبك أحد وأنت صائم",
};

const char azkar_title_Wwu_ar[Azkar_WWU_Sec][5] = 
{
    AzkarNullLogo,
    AzkarNullLogo,
    AzkarNullLogo,
};

const char azkar_title_Gar_ar[Azkar_Gar_Sec][30] = 
{
    "عند لبس الثوب",
    AzkarNullLogo,
    "قبل خلع الثوب",
};

const char azkar_title_Abu_ar[Azkar_Abu_Sec][45] = 
{
    "عند بدء الوضوء",
    "عند الفراغ من الوضوء",
    AzkarNullLogo,
};

const char azkar_title_Home_ar[Azkar_Home_Sec][50] =
{
    "عند دخول المنزل",
    "عند الخروج من المنزل",
    "قبل دخول الخلاء",
    "بعد الفراغ من الخلاء",
    "عند بدء الوضوء",
    "عند الإنتهاء من الوضوء",
    AzkarNullLogo,
};

const char azkar_title_Mos_ar[Azkar_Mos_Sec][60] =
{   
    "الدعاء عند الذهاب للمسجد",
    "الدعاء عند دخول المسجد",
    "الدعاء عند الخروج من المسجد",
};

const char azkar_title_Ath_ar[Azkar_Ath_Sec][45] =
{
    "عند سماع صوت الأذان",
    "بعد انتهاء الأذان",
};

const char azkar_title_Food_ar[Azkar_Food_Sec][45] =
{
    "قبل البدء بالأكل",
    "عند الفراغ من الأكل",
    "دعاء الضيف للمضيف",
};

const char azkar_title_Tra_ar[Azkar_Tra_Sec][45] = 
{
    "دعاء السفر",
    "دعاء المسافر للمقيم",
    "دعاء المقيم للمسافر",
};

const char azkar_title_OR_ar[Azkar_OR_Sec][65] = 
{
    "عند رؤية منام سيء",
    "دعاء قنوت الوتر",
    "دعاء الهم والكرب",
    AzkarNullLogo,
    "الدعاء لمن تعسر عليه أمر",
    "الدعاء عند عيادة مريض",
    AzkarNullLogo,
    "الدعاء لمن أصابته مصيبة",
    "الدعاء للميت في صلاة الجنازة",
    "الدعاء عند زيارة القبور",
    "الدعاء عند الغضب",
    "الدعاء لمن قدم لك من ماله",
    "الدعاء عند دخول السوق",
    "عند تلقي خبر مفرح أو محزن",
    AzkarNullLogo,
};

const char azkar_title_Pra_ar[Azkar_Pra_Sec][80] = 
{
    "الذكر بعد التكبير",
    "أثناء القيام",
    "أثناء الركوع",
    "عند الرفع من الركوع",
    "أثناء السجود",
    "أثناء الجلوس بين السجدتين",
    "التشهد: مايقال في وضع التشهد",
    "التشهد الأخير: مايقال في وضع التشهد الأخير",
    "مايقال بعد التشهد وقبل السلام",
};
#endif
