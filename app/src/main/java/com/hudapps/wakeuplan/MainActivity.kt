package com.hudapps.wakeuplan

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.hudapps.wakeuplan.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val macAddress = ""
        val broadcast = "192.168.100.255"

        binding.buttonSend.setOnClickListener {
            val result = sendMagicPacketJNI(macAddress, broadcast)
            binding.sampleText.text = result
        }
    }

    external fun sendMagicPacketJNI(macAddr: String, broadcast: String): String

    companion object {
        init {
            System.loadLibrary("wakeuplan")
        }
    }
}
